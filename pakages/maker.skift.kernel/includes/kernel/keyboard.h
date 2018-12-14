#pragma once

/* Copyright © 2018-2019 MAKER.                                               */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <skift/generic.h>

#include "kernel/shared_keyboard.h"

void keyboard_setup();

void keyborad_get_state(keyboard_state_t * state);