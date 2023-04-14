#include <karm-main/main.h>

Res<> entryPoint(Ctx &) {
    Sys::println("Hello, world!");
    return Ok();
}
