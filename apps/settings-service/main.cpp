#include <libsystem/Logger.h>
#include <libsystem/io/Socket.h>

#include <libsystem/eventloop/EventLoop.h>
#include <libsystem/eventloop/Notifier.h>

#include "settings-service/Server.h"

int main(int argc, const char **argv)
{
    __unused(argc);
    __unused(argv);

    logger_info("Initializing setting-service...");

    EventLoop::initialize();

    logger_info("Loading settings...");

    auto repository = Settings::Repository::load();

    logger_info("Starting server...");

    Settings::Server server{repository};

    logger_info("Ready!");

    return EventLoop::run();
}
