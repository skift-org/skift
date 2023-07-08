#include <karm-logger/logger.h>

#include "arch.h"
#include "mem.h"
#include "sched.h"

namespace Hjert::Core {

static Opt<Sched> _sched;

Res<> Sched::init(Handover::Payload &) {
    logInfo("sched: initializing...");
    auto bootTask = try$(Task::create(TaskMode::SUPER, try$(Space::create())));
    try$(bootTask->ready(0, 0, {}));
    _sched.emplace(std::move(bootTask));
    return Ok();
}

Sched &Sched::instance() {
    return *_sched;
}

Res<> Sched::enqueue(Strong<Task> task) {
    LockScope scope(_lock);
    _tasks.pushBack(std::move(task));
    return Ok();
}

void Sched::schedule(TimeSpan span) {
    LockScope scope(_lock);

    _stamp += span;
    _prev = _curr;
    _curr->_sliceEnd = _stamp;
    auto next = _idle;
    // HACK: to make sure the idle task is always scheduled last
    _idle->_sliceEnd = _stamp;

    for (usize i = 0; i < _tasks.len(); ++i) {
        auto &t = _tasks[i];

        if (t->hasRet()) {
            logInfo("{}: stopped", *t);
            _tasks.removeAt(i--);
            continue;
        }

        bool isUnblocked = not t->blocked() or t->unblock(_stamp);

        if (isUnblocked and Op::lteq(t->_sliceEnd, next->_sliceEnd)) {
            next = t;
        }
    }

    _curr = next;
}

void Sched::yield() {
    Arch::yield();
}

} // namespace Hjert::Core
