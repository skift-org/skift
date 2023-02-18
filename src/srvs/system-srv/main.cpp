#include <karm-main/main.h>
#include <karm-sys/chan.h>

Res<> entryPoint(CliArgs) {
    Sys::println("Hello from userspace!");
    return Ok();
}
