#include <karm-logger/logger.h>
#include <karm-main/main.h>
#include <karm-sys/file.h>

#include "loader.h"

Res<> entryPoint(Ctx &ctx) {
    logInfo("loading configs...");
    auto file = try$(Sys::File::open("file:/boot/loader.json"_url));

    logInfo("parsing configs...");
    auto fileStr = try$(Io::readAllUtf8(file));
    auto json = try$(Json::parse(fileStr));

    logInfo("validating configs...");
    logInfo("configs: {}", json);

    auto configs = try$(Loader::Configs::fromJson(json));

    if (configs.entries.len() > 1 or configs.entries.len() == 0)
        return Loader::showMenu(ctx, configs);

    return Loader::loadEntry(configs.entries[0]);
}
