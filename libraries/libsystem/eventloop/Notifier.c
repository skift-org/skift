/* Copyright © 2018-2020 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <libsystem/eventloop/EventLoop.h>
#include <libsystem/eventloop/Notifier.h>
#include <libsystem/logger.h>

void notifier_initialize(Notifier *notifier, Handle *handle, SelectEvent events)
{
    notifier->handle = handle;
    notifier->events = events;

    eventloop_register_notifier(notifier);
}

void notifier_uninitialize(Notifier *notifier)
{
    eventloop_unregister_notifier(notifier);
}

Notifier *notifier_create(Handle *handle, SelectEvent events)
{
    Notifier *notifier = __create(Notifier);

    notifier_initialize(notifier, handle, events);

    return notifier;
}

void notifier_destroy(Notifier *notifier)
{
    notifier_uninitialize(notifier);

    free(notifier);
}