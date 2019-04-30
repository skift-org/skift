#pragma once

#include "hideo/desktop.h"

typedef struct 
{
    list_t* desktops;
    hideo_desktop_t* current_desktop;
    hideo_surface_t* focused_surface;
} hideo_compositor_t;
