#pragma once

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