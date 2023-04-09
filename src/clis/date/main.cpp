#include <karm-main/main.h>
#include <karm-sys/time.h>

Res<> entryPoint(CliArgs) {
    Sys::println("{}", Sys::dateTime());
    return Ok();
}
