#pragma once

#include <handover/spec.h>
#include <karm-base/rc.h>
#include <karm-base/res.h>
#include <karm-base/time.h>
#include <karm-base/vec.h>

namespace Hjert::Core {

struct Task;

struct Sched {
    Instant _stamp{};
    Lock _lock{};

    Vec<Arc<Task>> _tasks;
    Arc<Task> _prev;
    Arc<Task> _curr;
    Arc<Task> _idle;

    Sched(Arc<Task> boot);

    Res<> enqueue(Arc<Task> task);

    void schedule(Duration span);
};

Res<> initSched(Handover::Payload &payload);

Sched &globalSched();

} // namespace Hjert::Core
