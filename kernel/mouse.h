#pragma once

/* Copyright © 2018-2019 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <libsystem/runtime.h>

typedef struct 
{
    int x;
    int y;
    int scroll;

    bool left;
    bool right;
    bool middle;
} mouse_state_t;

void mouse_setup();
void mouse_get_state(mouse_state_t *state);
void mouse_set_state(mouse_state_t *state);