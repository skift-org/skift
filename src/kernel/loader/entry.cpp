#include <json/json.h>
#include <karm-logger/logger.h>
#include <karm-main/main.h>
#include <karm-sys/file.h>

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

    auto file = try$(Sys::File::open("/boot/config.json"));

    auto configs = try$(Json::parse(file));

    logInfo("Configs: {}", configs);

    return Loader::load("/boot/kernel.elf");
}
