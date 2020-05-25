
#include <libsystem/eventloop/EventLoop.h>
#include <libsystem/eventloop/Notifier.h>
#include <libsystem/logger.h>

Notifier *notifier_create(
    void *target,
    Handle *handle,
    SelectEvent events,
    NotifierCallback callback)
{
    Notifier *notifier = __create(Notifier);

    notifier->target = target;
    notifier->handle = handle;
    notifier->events = events;
    notifier->callback = callback;

    eventloop_register_notifier(notifier);

    return notifier;
}

void notifier_destroy(Notifier *notifier)
{
    eventloop_unregister_notifier(notifier);
    free(notifier);
}
