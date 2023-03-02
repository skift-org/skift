#pragma once

#include <karm-base/time.h>

#include "task.h"

namespace Hjert::Core {

/* --- Sched ---------------------------------------------------------------- */

struct Sched {
    Tick _tick{};
    Lock _lock{};

    Vec<Strong<Task>> _tasks;
    Strong<Task> _curr;
    Strong<Task> _next;

    static Res<> init(Handover::Payload &);

    static Sched &self();

    Sched(Strong<Task> bootTask)
        : _tasks{bootTask}, _curr(bootTask), _next(bootTask) {
    }

    Res<> start(Strong<Task> task, uintptr_t ip) {
        return start(task, ip, task->stack().loadSp());
    }

    Res<> start(Strong<Task> task, uintptr_t ip, uintptr_t sp);

    void schedule();
};

} // namespace Hjert::Core
