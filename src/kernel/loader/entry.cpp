#include <karm-logger/logger.h>
#include <karm-main/main.h>

#include "loader.h"

CliResult entryPoint(CliArgs) {
    //
    //
    //
    // padding so line numbers line up in the output
    logInfo("");
    logInfo("  _                 _");
    logInfo(" | |   ___  __ _ __| |___ _ _");
    logInfo(" | |__/ _ \\/ _` / _` / -_) '_|");
    logInfo(" |____\\___/\\__,_\\__,_\\___|_|");
    logInfo("");

    return Loader::load("/boot/kernel.elf");
}
