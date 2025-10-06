import Karm.Logger;

#include "arch.h"
#include "sched.h"
#include "space.h"
#include "task.h"

namespace Hjert::Core {

static Opt<Sched> _sched;

Res<> initSched(Handover::Payload&) {
    logInfo("sched: initializing...");
    auto bootTask = try$(Task::create(Mode::SUPER, try$(Space::create())));
    bootTask->label("entry");
    try$(bootTask->ready(0, 0, {}));
    _sched.emplace(std::move(bootTask));
    return Ok();
}

Sched& globalSched() {
    return *_sched;
}

Sched::Sched(Arc<Task> boot)
    : _tasks{boot},
      _prev(boot),
      _curr(boot),
      _idle(boot) {
}

Res<> Sched::enqueue(Arc<Task> task) {
    LockScope _{_lock};
    _tasks.pushBack(std::move(task));
    return Ok();
}

void Sched::schedule(Duration span) {
    LockScope _{_lock};

    _stamp += span;
    _prev = _curr;
    _curr->_sliceEnd = _stamp;

    auto next = _idle;
    // NOTE: to make sure the idle task is always scheduled last
    _idle->_sliceEnd = _stamp + 1;

    for (usize i = 0; i < _tasks.len(); ++i) {
        auto& t = _tasks[i];
        auto state = t->eval(_stamp);
        if (state == State::EXITED) {
            logInfo("{}: exited", *t);
            _tasks.removeAt(i--);
        } else if (state == State::RUNNABLE and t->_sliceEnd <= next->_sliceEnd) {
            next = t;
        }
    }

    _curr = next;
}

} // namespace Hjert::Core
