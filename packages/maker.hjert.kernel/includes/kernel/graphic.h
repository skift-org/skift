#pragma once

/* Copyright © 2018-2019 MAKER.                                               */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <skift/generic.h>

typedef enum
{
    TMCOLOR_BLACK,
    TMCOLOR_RED,
    TMCOLOR_GREEN,
    TMCOLOR_YELLOW,
    TMCOLOR_BLUE,
    TMCOLOR_MAGENTA,
    TMCOLOR_CYAN,
    TMCOLOR_LIGHT_GREY,
    TMCOLOR_DARK_GREY,
    TMCOLOR_LIGHT_RED,
    TMCOLOR_LIGHT_GREEN,
    TMCOLOR_LIGHT_YELLOW,
    TMCOLOR_LIGHT_BLUE,
    TMCOLOR_LIGHT_MAGENTA,
    TMCOLOR_LIGHT_CYAN,
    TMCOLOR_WHITE,
} textmode_color_t;

void graphic_early_setup(uint width, uint height);
void graphic_setup();

void graphic_blit(uint *buffer);
void graphic_blit_region(uint *buffer, uint x, uint y, uint w, uint h);
void graphic_size(uint *width, uint *height);

void graphic_pixel(uint x, uint y, uint color);