#include <karm-logger/logger.h>
#include <karm-main/main.h>

Res<> entryPoint(CliArgs) {
    logInfo("Hello from system server!");
    return Ok();
}
