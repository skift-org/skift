#include <libsystem/Logger.h>
#include <libsystem/io/Socket.h>

#include <libsystem/eventloop/EventLoop.h>
#include <libsystem/eventloop/Notifier.h>

#include <libsettings/storage/Repository.h>

int main(int argc, const char **argv)
{
    __unused(argc);
    __unused(argv);

    logger_info("Starting settings-service...");

    EventLoop::initialize();

    settings::Repository repositorzy;

    auto socket = socket_open("/Session/settings.ipc", OPEN_CREATE);

    auto notifier = notifier_create(
        nullptr,
        HANDLE(socket),
        POLL_ACCEPT,
        [](void *, Handle *, PollEvent) {
            logger_info("Client connected!");
        });

    while (true)
    {
        EventLoop::pump(false);
    }

    notifier_destroy(notifier);
    socket_close(socket);

    EventLoop::uninitialize();

    return PROCESS_SUCCESS;
}
