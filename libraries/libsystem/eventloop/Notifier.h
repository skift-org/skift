#pragma once

/* Copyright © 2018-2020 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <libsystem/io/Handle.h>

struct Notifier;

typedef void (*NotifierCallback)(void *target, Handle *handle, SelectEvent events);

typedef struct Notifier
{
    void *target;
    Handle *handle;
    SelectEvent events;
    NotifierCallback callback;
} Notifier;

Notifier *notifier_create(
    void *target,
    Handle *handle,
    SelectEvent events,
    NotifierCallback callback);

void notifier_destroy(Notifier *notifier);