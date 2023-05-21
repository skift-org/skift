#include <karm-logger/logger.h>

#include "arch.h"
#include "mem.h"
#include "sched.h"

namespace Hjert::Core {

static Opt<Sched> _sched;

Res<> Sched::init(Handover::Payload &) {
    logInfo("sched: initializing...");
    _sched.emplace(try$(Task::create(TaskMode::SUPER, try$(Space::create()))));
    return Ok();
}

Sched &Sched::instance() {
    return *_sched;
}

Res<> Sched::start(Strong<Task> task, usize ip, usize sp, Hj::Args args) {
    logInfo("sched: starting task (ip: {x}, sp: {x})...", ip, sp);

    LockScope scope{_lock};
    Arch::start(*task, ip, sp, args);
    _tasks.pushBack(std::move(task));
    return Ok();
}

void Sched::schedule(TimeSpan span) {
    LockScope scope{_lock};

    _stamp += span;
    _curr->_sliceEnd = _stamp;
    auto next = _idle;

    for (usize i = 0; i < _tasks.len(); ++i) {
        auto &t = _tasks[i];

        if (t->hasRet()) {
            logInfo("sched: {} has returned", *t);
            _tasks.removeAt(i--);
            continue;
        }

        if (Op::lteq(t->_sliceEnd, _stamp)) {
            next = t;
        }
    }

    _curr = next;
}

void Sched::yield() {
    Arch::yield();
}

} // namespace Hjert::Core
