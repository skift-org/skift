#pragma once

/* Copyright © 2018-2019 MAKER.                                               */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <skift/generic.h>
#include "kernel/protocol.h"

void mouse_setup();
void mouse_get_state(mouse_state_t *state);
void mouse_set_state(mouse_state_t *state);