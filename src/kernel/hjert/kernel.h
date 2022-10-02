#pragma once

#include <hal/io.h>
#include <hal/pmm.h>
#include <hal/vmm.h>
#include <hjert-api/types.h>
#include <karm-base/lock.h>
#include <karm-base/rc.h>
#include <karm-base/ring.h>
#include <karm-base/var.h>
#include <karm-base/vec.h>

namespace Hjert {

struct _Object {
    uint64_t _id;
    Lock _lock;

    virtual ~_Object() = default;
};

struct Handle {
    Var<Strong<_Object>, Weak<_Object>> _link;

    Handle(Strong<_Object> strongLink);

    Handle(Weak<_Object> weakLink);

    static Result<Handle> makeStrong(Strong<_Object> link);

    static Result<Handle> makeWeak(Weak<_Object> link);
};

template <typename Crtp>
struct Object : public _Object {
};

struct Mem : public Object<Mem> {
    Var<
        Hal::PmmRange,
        Hal::DmaRange>
        _range;
};

struct Mmap {
    Hal::VmmRange _range;
    Strong<Mem> _Mem;
};

struct Space : public Object<Space> {
    Vec<Mmap> _mmaps;
    Strong<Hal::Vmm> _vmm;

    Result<Hal::VmmRange> map(Strong<Mem>);

    Result<Hal::VmmRange> unmap(Hal::VmmRange);
};

struct Domain {
    Vec<Handle> _handles;

    Error add(Handle);

    Error remove(Handle);

    Error remove(uint64_t);

    Result<Handle> lookup(uint64_t);
};

struct Cpu;

struct Thread {
    Cpu *_cpu;

    bool _inSyscall;
    bool _stopped;
    bool _blocked;
    bool _started;

    size_t _timeStart;
    size_t _timeEnd;

    bool runnable() const {
        return _started && (!_blocked || (_stopped && !_inSyscall));
    }

    bool running() const {
        return _cpu != nullptr;
    }

    bool pending() const {
        return !running() && runnable();
    }

    void attach(Cpu &cpu) {
        _cpu = &cpu;
    }

    void detach() {
        _cpu = nullptr;
    }
};

struct Channel {
    Ring<Api::Msg> _buf;
    Result<Api::Msg> recv();
    Error send(Api::Msg);
};

struct Context : public Object<Context> {
};

struct Task : public Object<Task> {
    Strong<Context> _context;
    Strong<Space> _space;

    Thread _thread;
    Domain _domain;
    Channel _channel;
};

struct Cpu {
    bool _present;

    bool _retained;
    int _retainedCount;

    OptStrong<Task> _idle;
    OptStrong<Task> _curr;
    OptStrong<Task> _next;

    virtual void _start(void (*entry)()) = 0;
    virtual void stop() = 0;

    virtual void enter() = 0;
    virtual void leave() = 0;

    virtual void retain() = 0;
    virtual void release() = 0;

    void boot(Strong<Task> task) {
        attach(task);
        _curr = task;
    }

    void attach(Strong<Task> task);

    void detach(Strong<Task> task);
};

struct Cpus {
};

struct Sched {
    Cpus _cpus;

    void start(Strong<Task>);

    void stop(Strong<Task>);

    void block(Strong<Task>);
};

} // namespace Hjert
