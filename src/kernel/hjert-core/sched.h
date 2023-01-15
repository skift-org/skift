#pragma once

#include <hal/heap.h>
#include <handover/spec.h>
#include <karm-base/lock.h>
#include <karm-base/rc.h>
#include <karm-base/size.h>
#include <karm-base/time.h>
#include <karm-base/vec.h>

namespace Hjert::Sched {

/* --- Stack ----------------------------------------------------------------- */

struct Stack {
    Hal::HeapMem _mem;
    uintptr_t _sp;

    void saveSp(uintptr_t sp) { _sp = sp; }
    uintptr_t loadSp() { return _sp; }

    void push(Bytes bytes) {
        _sp -= bytes.len();
        memcpy((void *)_sp, bytes.buf(), bytes.len());
    }

    template <typename T>
    void push(T t) {
        push(Bytes{reinterpret_cast<uint8_t *>(&t), sizeof(t)});
    }

    static constexpr size_t DEFAULT_SIZE = kib(16);

    static Result<Stack> create();
};

/* --- Task ----------------------------------------------------------------- */

struct Task {
    Stack _stack;

    Tick _sliceStart = 0;
    Tick _sliceEnd = 0;

    Task(Stack stack) : _stack(std::move(stack)) {
    }

    Stack &stack() { return _stack; }

    static Result<Strong<Task>> create();
};

/* --- Sched ---------------------------------------------------------------- */

struct Sched {
    Tick _tick{};
    Lock _lock{};

    Vec<Strong<Task>> _tasks;
    Strong<Task> _curr;
    Strong<Task> _next;

    Sched(Strong<Task> bootTask)
        : _tasks{bootTask}, _curr(bootTask), _next(bootTask) {
    }

    Error start(Strong<Task> task, uintptr_t ip) {
        return start(task, ip, task->stack().loadSp());
    }

    Error start(Strong<Task> task, uintptr_t ip, uintptr_t sp);

    void schedule();
};

Error init(Handover::Payload &);

Sched &sched();

Task &self();

} // namespace Hjert::Sched