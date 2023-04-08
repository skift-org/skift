#include <karm-main/main.h>
#include <karm-sys/mmap.h>

Res<> entryPoint(CliArgs) {
    Sys::println("Hello from system server!");

    return Ok();
}
