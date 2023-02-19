#include <karm-main/main.h>


Res<> entryPoint(CliArgs) {
    Sys::println("Hello from userspace!");
    return Ok();
}
