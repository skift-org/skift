#include <karm-logger/logger.h>

#include "arch.h"
#include "mem.h"
#include "sched.h"

namespace Hjert::Core {

static Opt<Sched> _sched;

Res<Box<Ctx>> Ctx::create(uintptr_t ksp) {
    return Arch::createCtx(ksp);
}

/* --- Stack ----------------------------------------------------------------- */

Res<Stack> Stack::create() {
    logInfo("sched: creating stack...");
    auto mem = try$(kmm().allocOwned(DEFAULT_SIZE));
    auto base = mem.range().end();
    return Ok(Stack{std::move(mem), base});
}

/* --- Task ----------------------------------------------------------------- */

Res<Strong<Task>> Task::create(TaskType type, Strong<Space> space) {
    logInfo("sched: creating task...");
    auto stack = try$(Stack::create());
    auto ctx = try$(Ctx::create(stack.loadSp()));
    auto task = makeStrong<Task>(type, std::move(stack), space, std::move(ctx));
    return Ok(task);
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
    _sched = Sched{try$(Task::create(TaskType::KERNEL, try$(Space::create())))};
    return Ok();
}

Sched &Sched::self() {
    return *_sched;
}

} // namespace Hjert::Core
