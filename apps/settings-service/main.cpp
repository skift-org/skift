#include <libsystem/Logger.h>
#include <libsystem/io/Socket.h>

#include <libsystem/eventloop/EventLoop.h>
#include <libsystem/eventloop/Notifier.h>

#include <libsettings/Repository.h>

#include "settings-service/Server.h"

int main(int argc, const char **argv)
{
    __unused(argc);
    __unused(argv);

    EventLoop::initialize();

    settings::Repository repository;

    settings::Server server{repository};

    return EventLoop::run();
}
