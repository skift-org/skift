#pragma once

/* Copyright © 2018-2019 MAKER.                                               */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <skift/generic.h>

#define HIDEO_KEYPRESSED
#define HIDEO_KEYRELESSED

typedef struct
{
    uint width;
    uint height;
} hideo_wincreate_t;

typedef struct 
{
    uint width;
    uint height;
} hideo_winresize_t;