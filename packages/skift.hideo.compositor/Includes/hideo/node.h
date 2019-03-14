#pragma once

#include <skift/list.h>
#include "hideo/surface.h"

typedef enum 
{
    HIDEO_VERTICAL,
    HIDEO_HORIZONTAL,
    HIDEO_TABED,
    HIDEO_STACKED
} hideo_node_layout_t;

typedef struct
{
    hideo_surface_t* surface;
    list_t* childs;
} hideo_node_t;
