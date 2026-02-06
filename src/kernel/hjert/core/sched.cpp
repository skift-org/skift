module;

#include <karm/macros>
#include <vaerk-handover/spec.h>

export module Hjert.Core:sched;

import Karm.Core;
import :clock;
import :task;

namespace Hjert::Core {

struct Task;

struct Sched {
    Lock _lock{};

    Vec<Arc<Task>> _tasks;
    Arc<Task> _prev;
    Arc<Task> _curr;
    Arc<Task> _idle;

    Task& currentTask() {
        return *_curr;
    }

    Sched(Arc<Task> boot)
        : _tasks{boot},
          _prev(boot),
          _curr(boot),
          _idle(boot) {
    }

    Res<> enqueue(Arc<Task> task) {
        LockScope _{_lock};
        _tasks.pushBack(std::move(task));
        return Ok();
    }

    void schedule() {
        LockScope _{_lock};

        auto now = clockNow();
        _prev = _curr;
        _curr->_sliceEnd = now;

        auto next = _idle;
        // NOTE: to make sure the idle task is always scheduled last
        _idle->_sliceEnd = now + 1;

        for (usize i = 0; i < _tasks.len(); ++i) {
            auto& t = _tasks[i];
            auto state = t->eval(now);
            if (state == State::EXITED) {
                logInfo("{}: exited", *t);
                _tasks.removeAt(i--);
            } else if (state == State::RUNNABLE and t->_sliceEnd <= next->_sliceEnd) {
                next = t;
            }
        }

        _curr = next;
    }
};

static Opt<Sched> _sched;

Res<> initSched() {
    logInfo("sched: initializing...");
    auto bootTask = try$(Task::create(Hj::Mode::SUPER, try$(Space::create())));
    bootTask->label("entry");
    try$(bootTask->ready(0, 0, {}));
    _sched.emplace(std::move(bootTask));
    return Ok();
}

Sched& globalSched() {
    return *_sched;
}

} // namespace Hjert::Core
