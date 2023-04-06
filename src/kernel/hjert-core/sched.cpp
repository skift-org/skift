#include <karm-logger/logger.h>

#include "arch.h"
#include "mem.h"
#include "sched.h"

namespace Hjert::Core {

static Opt<Sched> _sched;

Res<> Sched::start(Strong<Task> task, usize ip, usize sp) {
    logInfo("sched: starting task (ip: {x}, sp: {x})...", ip, sp);

    SchedLockScope scope;
    Arch::start(*task, ip, sp, {});
    _tasks.pushBack(std::move(task));
    return Ok();
}

void Sched::schedule(TimeSpan span) {
    SchedLockScope scope;

    _stamp += span;
    _curr->_sliceEnd = _stamp;
    auto next = _curr;

    for (auto &t : _tasks) {
        if (Op::lt(t->_sliceEnd, _stamp)) {
            next = t;
            break;
        }
    }
    _curr = next;
    _curr->_sliceStart = _stamp;
}

Res<> Sched::init(Handover::Payload &) {
    logInfo("sched: initializing...");
    _sched.emplace(try$(Task::create(TaskType::SUPER, try$(Space::create()))));
    return Ok();
}

Sched &Sched::instance() {
    return *_sched;
}

void Sched::yield() {
    {
        SchedLockScope scope;
    }
    Arch::yield();
}

} // namespace Hjert::Core
