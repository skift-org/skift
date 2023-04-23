#include <karm-logger/logger.h>
#include <karm-main/main.h>
#include <karm-sys/file.h>

#include "loader.h"

Res<> entryPoint(Ctx &ctx) {
    logInfo("");
    logInfo("  _                 _");
    logInfo(" | |   ___  __ _ __| |___ _ _");
    logInfo(" | |__/ _ \\/ _` / _` / -_) '_|");
    logInfo(" |____\\___/\\__,_\\__,_\\___|_|");
    logInfo("");

    logInfo("loading configs...");
    auto file = try$(Sys::File::open("file:/boot/loader.json"_url));

    logInfo("parsing configs...");
    auto fileStr = try$(Io::readAllUtf8(file));
    auto json = try$(Json::parse(fileStr));

    logInfo("validating configs...");
    logInfo("configs: {}", json);

    auto configs = try$(Loader::Configs::fromJson(json));

    if (configs.entries.len() > 1 || configs.entries.len() == 0)
        return Loader::showMenu(ctx, configs);

    return Loader::loadEntry(configs.entries[0]);
}
