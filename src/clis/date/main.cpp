#include <karm-sys/entry.h>
#include <karm-sys/time.h>

Res<> entryPoint(Sys::Ctx &) {
    Sys::println("{}", Sys::dateTime());
    return Ok();
}
