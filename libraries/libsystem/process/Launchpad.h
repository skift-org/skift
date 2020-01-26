#pragma once

#include "abi/Launchpad.h"

Launchpad *launchpad_create(const char *name, const char *executable);

void launchpad_destroy(Launchpad *launchpad);

void launchpad_argument(Launchpad *launchpad, const char *argument);

int launchpad_launch(Launchpad *Launchpad);