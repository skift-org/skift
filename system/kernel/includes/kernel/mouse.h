#pragma once

/* Copyright © 2018-2019 MAKER.                                               */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <skift/generic.h>
#include "kernel/protocol.h"

typedef PACKED(struct) 
{
    int x;
    int y;
    int scroll;

    bool left;
    bool right;
    bool middle;
} mouse_state_t;

void mouse_setup();