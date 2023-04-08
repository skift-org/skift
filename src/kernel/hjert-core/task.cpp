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
    // NOTE: Can't use ObjectLockScope here because we need to yield
    //       outside of the lock.
    _lock.acquire();
    _block = std::move(blocker);
    _lock.release();
    Sched::instance().yield();
    return Ok();
}

bool Task::unblock(TimeStamp now) {
    ObjectLockScope scope(*this);

    if (_block) {
        if (Op::gt((*_block)(), now)) {
            return false;
        }
        _block = NONE;
    }

    return false;
}

void Task::crash() {
    logError("task: crashed");
    ObjectLockScope scope(*this);
    _ret = -1;
}

void Task::ret(Hj::Arg val) {
    logInfo("task: returning from task");
    ObjectLockScope scope(*this);
    _ret = val;
}

Res<Hj::Arg> Task::wait(Strong<Task> task) {
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
    ObjectLockScope scope(*this);
    _mode = TaskMode::SUPER;
}

void Task::leaveSupervisorMode() {
    // NOTE: Can't use ObjectLockScope here because we need to yield
    //       outside of the lock.
    _lock.acquire();
    _mode = TaskMode::USER;
    bool shouldYield = _ret;
    _lock.release();

    if (shouldYield)
        Sched::instance().yield();
}

void Task::enterIdleMode() {
    ObjectLockScope scope(*this);
    _mode = TaskMode::IDLE;
}

} // namespace Hjert::Core
