#include <libsystem/eventloop/EventLoop.h>
#include <libsystem/eventloop/Notifier.h>

#include <libsystem/logger.h>

Notifier *notifier_create(Handle *handle, SelectEvent events)
{
    Notifier *notifier = __create(Notifier);

    notifier->handle = handle;
    notifier->events = events;

    eventloop_register_notifier(notifier);

    return notifier;
}

void notifier_destroy(Notifier *notifier)
{
    eventloop_unregister_notifier(notifier);

    free(notifier);
}