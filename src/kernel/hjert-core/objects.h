#pragma once

#include <hal/io.h>
#include <hal/vmm.h>
#include <hjert-api/types.h>
#include <karm-base/atomic.h>
#include <karm-base/checked.h>
#include <karm-base/func.h>
#include <karm-base/lock.h>
#include <karm-base/range-alloc.h>
#include <karm-base/rc.h>
#include <karm-base/ring.h>
#include <karm-fmt/case.h>
#include <karm-logger/logger.h>

#include "ctx.h"

namespace Hjert::Core {

/* --- Object --------------------------------------------------------------- */

struct Object : public Meta::Static {
    static Atomic<usize> _counter;

    Lock _lock;
    usize _id = _counter.fetchAdd(1);
    Opt<String> _label;
    Flags<Hj::Sigs> _signals;

    virtual ~Object() = default;

    virtual Hj::Type type() const = 0;

    usize id() const { return _id; }

    void label(Str label);

    Str label() const;

    void _signalUnlock(Flags<Hj::Sigs> set, Flags<Hj::Sigs> unset);

    Flags<Hj::Sigs> _pollUnlock();

    void signal(Flags<Hj::Sigs> set, Flags<Hj::Sigs> unset);

    Flags<Hj::Sigs> poll();
};

template <typename Crtp, Hj::Type _TYPE>
struct BaseObject : public Object {
    using _Crtp = Crtp;
    static constexpr Hj::Type TYPE = _TYPE;
    using Object::Object;

    Hj::Type type() const override {
        return TYPE;
    }
};

struct ObjectLockScope : public LockScope {
    ObjectLockScope(Object &obj)
        : LockScope(obj._lock) {
    }
};

/* --- Domain --------------------------------------------------------------- */

struct Domain : public BaseObject<Domain, Hj::Type::DOMAIN> {
    using Slot = Opt<Strong<Object>>;

    static constexpr usize SHIFT = 11;
    static constexpr usize LEN = 2 << 11;
    static constexpr usize MASK = LEN - 1;

    Array<Slot, LEN> _slots;

    static Res<Strong<Domain>> create();

    Res<Hj::Cap> add(Hj::Cap dest, Strong<Object> obj);

    Res<Strong<Object>> get(Hj::Cap cap);

    template <typename T>
    Res<Strong<T>> get(Hj::Cap cap) {
        auto obj = try$(get(cap));
        if (not obj.is<T>()) {
            return Error::invalidHandle("type missmatch");
        }
        return Ok(try$(obj.cast<T>()));
    }

    Res<> drop(Hj::Cap cap);
};

/* --- Vmo ------------------------------------------------------------------ */

struct VNode : public BaseObject<VNode, Hj::Type::VMO> {
    using _Mem = Var<Hal::PmmMem, Hal::DmaRange>;
    _Mem _mem;

    static Res<Strong<VNode>> alloc(usize size, Hj::VmoFlags);

    static Res<Strong<VNode>> makeDma(Hal::DmaRange prange);

    VNode(_Mem mem) : _mem(std::move(mem)) {}

    Hal::PmmRange range();

    bool isDma() {
        return _mem.is<Hal::DmaRange>();
    }
};

/* --- Space ---------------------------------------------------------------- */

struct Space : public BaseObject<Space, Hj::Type::SPACE> {
    struct Map {
        Hal::VmmRange vrange;
        usize off;
        Strong<VNode> vmo;
    };

    Strong<Hal::Vmm> _vmm;
    RangeAlloc<Hal::VmmRange> _alloc;
    Vec<Map> _maps;

    static Res<Strong<Space>> create();

    Space(Strong<Hal::Vmm> vmm);

    ~Space() override;

    Res<usize> _lookup(Hal::VmmRange vrange);

    Res<> _validate(Hal::VmmRange vrange) {
        for (auto &map : _maps) {
            if (map.vrange.contains(vrange)) {
                return Ok();
            }
        }

        return Error::invalidInput("bad address");
    }

    Res<Hal::VmmRange> map(Hal::VmmRange vrange, Strong<VNode> vmo, usize off, Hj::MapFlags flags);

    Res<> unmap(Hal::VmmRange vrange);

    void activate();
};

/* --- Channel -------------------------------------------------------------- */

struct Parcel {
    using Slot = Var<None, Hj::Arg, Strong<Object>>;

    Hj::Arg _label;
    Hj::Arg _flags;
    Array<Slot, 6> _slots = makeArray<Slot, 6>(NONE);

    static Res<Parcel> fromMsg(Domain &dom, Hj::Msg msg);
    Res<Hj::Msg> toMsg(Domain &dom);
};

struct Channel :
    public BaseObject<Channel, Hj::Type::CHANNEL> {

    bool _closed = false;
    Ring<Parcel> _ring;
    usize _cap;

    static Res<Strong<Channel>> create(usize cap = 4096);

    Channel(usize cap);

    Res<> _ensureNoEmpty();

    Res<> _ensureNoFull();

    Res<> _ensureOpen();

    void _updateSignalsUnlock();

    Res<> send(Domain &dom, Hj::Msg msg);

    Res<Hj::Msg> recv(Domain &dom);

    Res<> close();
};

/* --- Listener ------------------------------------------------------------- */

struct Listener :
    public BaseObject<Listener, Hj::Type::LISTENER> {

    struct Listened {
        Hj::Cap cap;
        Strong<Object> obj;

        Flags<Hj::Sigs> set;
        Flags<Hj::Sigs> unset;
    };

    Vec<Listened> _listened;
    Vec<Hj::Event> _events;

    static Res<Strong<Listener>> create();

    Res<> watch(Hj::Cap cap, Strong<Object> obj, Flags<Hj::Sigs> set, Flags<Hj::Sigs> unset);

    Slice<Hj::Event> listen();

    Slice<Hj::Event> events() {
        return _events;
    }
};

/* --- Task ----------------------------------------------------------------- */

enum struct TaskMode : u8 {
    IDLE,  // The task is only run when there is no other task to run
    USER,  // The task is running in user mode
    SUPER, // The task is running in supervisor mode propably serving a syscall
};

using Blocker = Func<TimeStamp()>;

struct Task :
    public BaseObject<Task, Hj::Type::TASK> {

    TaskMode _mode;
    Stack _stack;
    Box<Ctx> _ctx;

    Opt<Strong<Space>> _space;
    Opt<Strong<Domain>> _domain;
    Opt<Blocker> _block;

    TimeStamp _sliceEnd = 0;

    bool _hasRetUnlock() {
        return _signals.has(Hj::Sigs::EXITED) and
               _mode == TaskMode::USER;
    }

    bool hasRet() {
        ObjectLockScope scope(*this);
        return _hasRetUnlock();
    }

    bool runable() {
        ObjectLockScope scope(*this);
        return not _block and not _hasRetUnlock();
    }

    static Res<Strong<Task>> create(
        TaskMode mode,
        Opt<Strong<Space>> space = NONE,
        Opt<Strong<Domain>> domain = NONE);

    static Task &self();

    Task(TaskMode mode,
         Stack stack,
         Box<Ctx> ctx,
         Opt<Strong<Space>> space,
         Opt<Strong<Domain>> domain)
        : _mode(mode),
          _stack(std::move(stack)),
          _ctx(std::move(ctx)),
          _space(space),
          _domain(domain) {
    }

    Stack &stack() { return _stack; }

    Space &space() { return *_space.unwrap(); }

    Domain &domain() { return *_domain.unwrap(); }

    TaskMode mode() const { return _mode; }

    bool blocked() const { return _block; }

    void saveCtx(usize sp) {
        _stack.saveSp(sp);
        _ctx->save();
    }

    usize loadCtx() {
        if (_space)
            (*_space)->activate();

        _ctx->load();
        return _stack.loadSp();
    }

    Res<> block(Blocker blocker);

    bool unblock(TimeStamp now);

    void crash();

    void enterSupervisorMode();

    void leaveSupervisorMode();

    void enterIdleMode();
};

/* --- I/O ------------------------------------------------------------------ */

struct IoNode :
    public BaseObject<IoNode, Hj::Type::IO> {

    Hal::PortRange _range;

    static Res<Strong<IoNode>> create(Hal::PortRange range);

    IoNode(Hal::PortRange range);

    Res<Hj::Arg> in(usize offset, usize size);

    Res<> out(usize offset, usize size, Hj::Arg arg);
};

/* --- IRQ ------------------------------------------------------------------ */

struct Irq :
    public BaseObject<Irq, Hj::Type::IRQ> {

    usize _irq;

    static Res<Strong<Irq>> create(usize irq);

    static void trigger(usize irq);

    Irq(usize irq);

    ~Irq() override;
};

} // namespace Hjert::Core

template <Meta::Derive<Hjert::Core::Object> T>
struct Karm::Fmt::Formatter<T> {
    Res<usize> format(Io::TextWriter &writer, Hjert::Core::Object const &obj) {
        return Fmt::format(writer, "{}({}, '{}')", Fmt::toPascalCase(Hj::toStr(obj.type())).unwrap(), obj.id(), obj.label());
    }
};
