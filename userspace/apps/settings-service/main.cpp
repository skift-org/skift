#include <libasync/Loop.h>
#include <libsystem/Logger.h>

#include "settings-service/Server.h"

int main(int argc, const char **argv)
{
    UNUSED(argc);
    UNUSED(argv);

    logger_info("Initializing setting-service...");

    logger_info("Loading settings...");

    auto repository = Settings::Repository::load();

    logger_info("Starting server...");

    Settings::Server server{repository};

    logger_info("Ready!");

    return Async::Loop::the()->run();
}
