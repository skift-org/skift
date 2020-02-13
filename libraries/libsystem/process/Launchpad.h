#pragma once

/* Copyright © 2018-2020 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <abi/Launchpad.h>

Launchpad *launchpad_create(const char *name, const char *executable);

void launchpad_destroy(Launchpad *launchpad);

void launchpad_argument(Launchpad *launchpad, const char *argument);

void launchpad_handle(Launchpad *launchpad, Handle *handle_to_pass, int destination);

int launchpad_launch(Launchpad *Launchpad);
