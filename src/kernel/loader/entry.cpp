#include <json/json.h>
#include <karm-logger/logger.h>
#include <karm-main/main.h>
#include <karm-sys/file.h>

#include "loader.h"

Res<> entryPoint(CliArgs) {
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

    logInfo("Loading configs...");
    auto file = try$(Sys::File::open("/boot/loader.json"));

    logInfo("Parsing configs...");
    auto fileStr = try$(Io::readAllUtf8(file));
    auto json = try$(Json::parse(fileStr));

    logInfo("Validating configs...");
    logInfo("Configs: {}", json);

    auto configs = try$(Loader::Configs::fromJson(json));

    return Loader::load("/boot/kernel.elf", configs.entries[0]);
}
