#include <karm-logger/logger.h>

#include "arch.h"
#include "ctx.h"
#include "mem.h"

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

void Stack::saveSp(usize sp) {
    _sp = sp;
}

usize Stack::loadSp() {
    return _sp;
}

void Stack::push(Bytes bytes) {
    _sp -= bytes.len();
    memcpy((void *)_sp, bytes.buf(), bytes.len());
}

} // namespace Hjert::Core
