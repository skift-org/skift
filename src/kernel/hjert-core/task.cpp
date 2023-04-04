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
    logInfo("sched: creating stack...");
    auto mem = try$(kmm().allocOwned(DEFAULT_SIZE));
    auto base = mem.range().end();
    return Ok(Stack{std::move(mem), base});
}

/* --- Task ----------------------------------------------------------------- */

Res<Strong<Task>> Task::create(
    TaskType type, OptStrong<Space> space, OptStrong<Domain> domain) {

    logInfo("sched: creating task...");
    auto stack = try$(Stack::create());
    auto ctx = try$(Ctx::create(stack.loadSp()));
    auto task = makeStrong<Task>(type, std::move(stack), std::move(ctx), space, domain);
    return Ok(task);
}

Task &Task::self() {
    return *Sched::instance()._curr;
}

} // namespace Hjert::Core
