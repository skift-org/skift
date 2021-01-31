#include <libsystem/Logger.h>
#include <libsystem/io/Socket.h>

#include <libsystem/eventloop/EventLoop.h>
#include <libsystem/eventloop/Notifier.h>

#include "settings-service/Server.h"

int main(int argc, const char **argv)
{
    __unused(argc);
    __unused(argv);

    EventLoop::initialize();

    auto repository = settings::Repository::load();

    settings::Server server{repository};

    return EventLoop::run();
}
