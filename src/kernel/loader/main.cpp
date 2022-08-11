#include <karm-main/main.h>

#include "loader.h"

CliResult entryPoint(CliArgs) {
    return Loader::load("/boot/kernel.elf");
}
