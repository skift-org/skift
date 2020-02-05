#pragma once

#include <libsystem/io/Handle.h>

struct Notifier;

typedef void (*NotifierHandler)(struct Notifier *notifier, Handle *handle);

typedef struct Notifier
{
    Handle *handle;

    SelectEvent events;

    NotifierHandler on_ready_to_read;
    NotifierHandler on_ready_to_write;
    NotifierHandler on_ready_to_send;
    NotifierHandler on_ready_to_receive;
    NotifierHandler on_ready_to_connect;
    NotifierHandler on_ready_to_accept;
} Notifier;

Notifier *notifier_create(Handle *handle, SelectEvent events);

void notifier_destroy(Notifier *notifier);