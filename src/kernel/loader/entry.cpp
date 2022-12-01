#include <karm-debug/logger.h>
#include <karm-main/main.h>

#include "loader.h"

CliResult entryPoint(CliArgs) {
    //
    //
    //
    // padding so line numbers line up in the output
    Debug::linfo("");
    Debug::linfo("  _                 _");
    Debug::linfo(" | |   ___  __ _ __| |___ _ _");
    Debug::linfo(" | |__/ _ \\/ _` / _` / -_) '_|");
    Debug::linfo(" |____\\___/\\__,_\\__,_\\___|_|");
    Debug::linfo("");

    return Loader::load("/boot/kernel.elf");
}
