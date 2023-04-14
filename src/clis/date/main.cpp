#include <karm-main/main.h>
#include <karm-sys/time.h>

Res<> entryPoint(Ctx &) {
    Sys::println("{}", Sys::dateTime());
    return Ok();
}
