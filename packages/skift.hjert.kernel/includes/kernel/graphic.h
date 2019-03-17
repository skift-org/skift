#pragma once

/* Copyright © 2018-2019 N. Van Bossuyt.                                               */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <skift/generic.h>

#include "kernel/multiboot.h"


enum
{
GRAPHIC_CONTROL_BLIT,
GRAPHIC_CONTROL_GET_INFO,
GRAPHIC_CONTROL_SET_INFO,
};

typedef struct 
{
    uint height;
    uint width;

    struct
    {
        uint height;
        uint width;
    } text_mode;
} graphic_info_t;


void graphic_early_setup(multiboot_info_t* mbootinfo);
void graphic_setup(void);