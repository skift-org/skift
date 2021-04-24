#include <libasync/Loop.h>
#include <libio/Streams.h>

#include "settings-service/Server.h"

int main(int argc, const char **argv)
{
    UNUSED(argc);
    UNUSED(argv);

    IO::logln("Initializing setting-service...");

    IO::logln("Loading settings...");

    auto repository = Settings::Repository::load();

    IO::logln("Starting server...");

    Settings::Server server{repository};

    IO::logln("Ready!");

    return Async::Loop::the()->run();
}
