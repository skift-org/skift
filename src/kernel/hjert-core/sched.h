#pragma once

#include <handover/spec.h>
#include <karm-base/time.h>

#include "objects.h"

namespace Hjert::Core {

/* --- Sched ---------------------------------------------------------------- */

struct Sched {
    TimeStamp _stamp{};
    Lock _lock{};

    Vec<Strong<Task>> _tasks;
    Strong<Task> _curr;
    Strong<Task> _idle;

    static Res<> init(Handover::Payload &);

    static Sched &instance();

    static Lock &lock() {
        return instance()._lock;
    }

    Sched(Strong<Task> bootTask)
        : _tasks{bootTask},
          _curr(bootTask),
          _idle(bootTask) {
    }

    Res<> start(Strong<Task> task, usize ip, Hj::Args args) {
        return start(task, ip, task->stack().loadSp(), args);
    }

    Res<> start(Strong<Task> task, usize ip, usize sp, Hj::Args args);

    void schedule(TimeSpan span);

    void yield();
};

} // namespace Hjert::Core
