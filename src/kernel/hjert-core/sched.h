#pragma once

#include <handover/spec.h>
#include <karm-base/rc.h>
#include <karm-base/res.h>
#include <karm-base/time.h>
#include <karm-base/vec.h>

namespace Hjert::Core {

struct Task;

struct Sched {
    TimeStamp _stamp{};
    Lock _lock{};

    Vec<Strong<Task>> _tasks;
    Strong<Task> _prev;
    Strong<Task> _curr;
    Strong<Task> _idle;

    Sched(Strong<Task> boot);

    Res<> enqueue(Strong<Task> task);

    void schedule(TimeSpan span);
};

Res<> initSched(Handover::Payload &payload);

Sched &globalSched();

} // namespace Hjert::Core
