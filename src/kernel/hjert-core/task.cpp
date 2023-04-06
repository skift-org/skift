#include <karm-logger/logger.h>

#include "arch.h"
#include "sched.h"
#include "task.h"

namespace Hjert::Core {

Res<Box<Ctx>> Ctx::create(usize ksp) {
    return Arch::createCtx(ksp);
}

/* --- Stack ----------------------------------------------------------------- */

Res<Stack> Stack::create() {
    logInfo("task: creating stack...");
    auto mem = try$(kmm().allocOwned(DEFAULT_SIZE));
    auto base = mem.range().end();
    return Ok(Stack{std::move(mem), base});
}

/* --- Task ----------------------------------------------------------------- */

Res<Strong<Task>> Task::create(
    TaskType type, Opt<Strong<Space>> space, Opt<Strong<Domain>> domain) {

    logInfo("task: creating task...");
    auto stack = try$(Stack::create());
    auto ctx = try$(Ctx::create(stack.loadSp()));
    auto task = makeStrong<Task>(type, std::move(stack), std::move(ctx), space, domain);
    return Ok(task);
}

Task &Task::self() {
    return *Sched::instance()._curr;
}

Res<> Task::block(Blocker blocker) {
    ObjectLockScope scope(*this);

    _block = std::move(blocker);
    Sched::instance().yield();
    return Ok();
}

void Task::crash() {
    ObjectLockScope scope(*this);

    logError("task: crashed");
    _ret = -1;
    Sched::instance().yield();
}

void Task::ret(Hj::Arg val) {
    ObjectLockScope scope(*this);

    logInfo("task: returning from task");
    _ret = val;
    Sched::instance().yield();
}

Res<Hj::Arg> Task::wait(Strong<Task> task) {
    ObjectLockScope scope(*this);

    Hj::Arg ret;
    try$(block([&]() {
        if (not task->_ret)
            return TimeStamp::endOfTime();

        ret = task->_ret.unwrap();
        return TimeStamp::epoch();
    }));
    return Ok(ret);
}

void Task::enterSupervisorMode() {
    _mode = TaskMode::SUPER;
}

void Task::leaveSupervisorMode() {
    _mode = TaskMode::USER;
}

} // namespace Hjert::Core
