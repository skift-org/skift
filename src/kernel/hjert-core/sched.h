#pragma once

import Karm.Core;

#include <vaerk-handover/spec.h>

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

Res<> initSched(Handover::Payload& payload);

Sched& globalSched();

} // namespace Hjert::Core
