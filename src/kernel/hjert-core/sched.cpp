#include <karm-logger/logger.h>

#include "mem.h"
#include "sched.h"

namespace Hjert::Sched {

/* --- Stack ----------------------------------------------------------------- */

Result<Stack> Stack::create() {
    logInfo("sched: creating stack...");
    auto mem = try$(Mem::heap().allocOwned(DEFAULT_SIZE));
    auto base = mem.range().end();
    return Stack{std::move(mem), base};
}

/* --- Task ----------------------------------------------------------------- */

Result<Strong<Task>> Task::create() {
    logInfo("sched: creating task...");
    return makeStrong<Task>(try$(Stack::create()));
}

/* --- Sched ---------------------------------------------------------------- */

Error Sched::start(Strong<Task> task, uintptr_t ip, uintptr_t sp) {
    logInfo("sched: starting task (ip: {x}, sp: {x})...", ip, sp);
    LockScope scope(_lock);

    _tasks.pushBack(std::move(task));
    return OK;
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

static Opt<Sched> _sched;

Error init(Handover::Payload &) {
    logInfo("sched: initializing...");
    _sched = Sched{try$(Task::create())};
    return OK;
}

Sched &sched() {
    return *_sched;
}

Task &self() {
    return *_sched->_curr;
}

} // namespace Hjert::Sched