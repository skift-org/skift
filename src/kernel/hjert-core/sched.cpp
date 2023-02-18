#include <karm-logger/logger.h>

#include "arch.h"
#include "mem.h"
#include "sched.h"

namespace Hjert::Core {

static Opt<Sched> _sched;

/* --- Stack ----------------------------------------------------------------- */

Res<Stack> Stack::create() {
    logInfo("sched: creating stack...");
    auto mem = try$(Mem::heap().allocOwned(DEFAULT_SIZE));
    auto base = mem.range().end();
    return Ok(Stack{std::move(mem), base});
}

/* --- Task ----------------------------------------------------------------- */

Res<Strong<Task>> Task::create() {
    logInfo("sched: creating task...");
    return Ok(makeStrong<Task>(try$(Stack::create())));
}

Task &Task::self() {
    return *_sched->_curr;
}

/* --- Sched ---------------------------------------------------------------- */

Res<> Sched::start(Strong<Task> task, uintptr_t ip, uintptr_t sp) {
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
    _sched = Sched{try$(Task::create())};
    return Ok();
}

Sched &Sched::self() {
    return *_sched;
}

} // namespace Hjert::Core
