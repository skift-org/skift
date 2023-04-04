#include <karm-logger/logger.h>

#include "arch.h"
#include "mem.h"
#include "sched.h"

namespace Hjert::Core {

static Opt<Sched> _sched;

Res<> Sched::start(Strong<Task> task, usize ip, usize sp) {
    logInfo("sched: starting task (ip: {x}, sp: {x})...", ip, sp);
    LockScope scope(_lock);
    Arch::start(*task, ip, sp, {});
    _tasks.pushBack(std::move(task));
    return Ok();
}

void Sched::schedule() {
    LockScope scope(_lock);

    _tick++;

    _curr->_sliceEnd = _tick;
    auto next = _curr;

    for (auto &t : _tasks) {
        if (t->_sliceEnd < _tick) {
            next = t;
            break;
        }
    }

    _curr = next;
    _curr->_sliceStart = _tick;
}

Res<> Sched::init(Handover::Payload &) {
    logInfo("sched: initializing...");
    _sched.emplace(try$(Task::create(TaskType::SUPER, try$(Space::create()))));
    return Ok();
}

Sched &Sched::instance() {
    return *_sched;
}

} // namespace Hjert::Core
