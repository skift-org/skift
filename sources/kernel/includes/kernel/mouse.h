#pragma once

#include <skift/types.h>
#include "kernel/shared_mouse.h"

void mouse_setup();

void mouse_get_state(mouse_state_t* state);
void mouse_set_state(mouse_state_t* state);