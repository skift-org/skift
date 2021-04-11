#pragma once

#include <abi/Handle.h>
#include <abi/Launchpad.h>

#include <libio/Handle.h>
#include <libutils/String.h>
#include <libutils/Vector.h>

Launchpad *launchpad_create(const char *name, const char *executable);

void launchpad_destroy(Launchpad *launchpad);

void launchpad_flags(Launchpad *launchpad, TaskFlags flags);

void launchpad_argument(Launchpad *launchpad, const char *argument);

void launchpad_arguments(Launchpad *launchpad, const Vector<String> &arguments);

void launchpad_environment(Launchpad *launchpad, const char *buffer);

void launchpad_handle(Launchpad *launchpad, IO::RawHandle &handle_to_pass, int destination);

void launchpad_handle(Launchpad *launchpad, IO::Handle &handle, int destination);

void launchpad_handle(Launchpad *launchpad, Handle *handle_to_pass, int destination);

Result launchpad_launch(Launchpad *launchpad, int *pid);
