#pragma once

#include <karm-base/box.h>
#include <karm-base/func.h>
#include <karm-base/size.h>
#include <karm-base/time.h>

#include "space.h"

namespace Hjert::Core {

struct Ctx {
    static Res<Box<Ctx>> create(usize ksp);

    virtual ~Ctx() = default;
    virtual void save() = 0;
    virtual void load() = 0;
};

/* --- Stack ---------------------------------------------------------------- */

struct Stack {
    static constexpr usize DEFAULT_SIZE = kib(16);

    static Res<Stack> create();

    Hal::KmmMem _mem;
    usize _sp;

    void saveSp(usize sp) {
        _sp = sp;
    }

    usize loadSp() {
        return _sp;
    }

    void push(Bytes bytes) {
        _sp -= bytes.len();
        memcpy((void *)_sp, bytes.buf(), bytes.len());
    }

    template <typename T>
    void push(T t) {
        push(Bytes{reinterpret_cast<u8 *>(&t), sizeof(t)});
    }
};

/* --- Task ----------------------------------------------------------------- */

enum struct TaskType : u8 {
    IDLE, // The idle task that runs when there is no other task to run
    USER, // A user task
    SUPER // A a task that is running in supervisor mode
};

enum struct TaskMode : u8 {
    SUPER, // The task is running in supervisor mode propably serving a syscall
    USER   // The task is running in user mode
};

using Blocker = Func<TimeStamp()>;

struct Task : public BaseObject<Task> {
    TaskType _type;
    TaskMode _mode;
    Stack _stack;
    Box<Ctx> _ctx;

    Opt<Strong<Space>> _space;
    Opt<Strong<Domain>> _domain;
    Opt<Blocker> _block;

    TimeStamp _sliceStart = 0;
    TimeStamp _sliceEnd = 0;
    Opt<Hj::Arg> _ret;

    static Res<Strong<Task>> create(
        TaskType type,
        Opt<Strong<Space>> space = NONE,
        Opt<Strong<Domain>> domain = NONE);

    static Task &self();

    Task(TaskType type,
         Stack stack,
         Box<Ctx> ctx,
         Opt<Strong<Space>> space,
         Opt<Strong<Domain>> domain)
        : _type(type),
          _stack(std::move(stack)),
          _ctx(std::move(ctx)),
          _space(space),
          _domain(domain) {
    }

    auto type() const { return _type; }

    Stack &stack() { return _stack; }

    Space &space() { return *_space.unwrap(); }

    Domain &domain() { return *_domain.unwrap(); }

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

    void crash();

    void ret(Hj::Arg val);

    Res<Hj::Arg> wait(Strong<Task> task);

    void enterSupervisorMode();

    void leaveSupervisorMode();
};

} // namespace Hjert::Core
