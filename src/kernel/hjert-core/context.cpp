#include <karm-logger/logger.h>

#include "arch.h"
#include "context.h"
#include "mem.h"

namespace Hjert::Core {

// MARK: Stack ------------------------------------------------------------------

Res<Stack> Stack::create() {
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
