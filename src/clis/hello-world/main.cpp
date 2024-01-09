#include <karm-sys/entry.h>

Res<> entryPoint(Sys::Ctx &) {
    Sys::println("Hello, world!");
    return Ok();
}
