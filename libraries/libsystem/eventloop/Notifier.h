#pragma once

/* Copyright © 2018-2020 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

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

#define NOTIFIER(__subclass) ((Notifier *)(__subclass))

void notifier_initialize(Notifier *notifier, Handle *handle, SelectEvent events);

void notifier_uninitialize(Notifier *notifier);

Notifier *notifier_create(Handle *handle, SelectEvent events);

void notifier_destroy(Notifier *notifier);