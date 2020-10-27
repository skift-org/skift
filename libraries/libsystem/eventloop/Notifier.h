#pragma once

#include <libsystem/io/Handle.h>

struct Notifier;

typedef void (*NotifierCallback)(void *target, Handle *handle, PollEvent events);

struct Notifier
{
    void *target;
    Handle *handle;
    PollEvent events;
    NotifierCallback callback;
};

Notifier *notifier_create(
    void *target,
    Handle *handle,
    PollEvent events,
    NotifierCallback callback);

void notifier_destroy(Notifier *notifier);
