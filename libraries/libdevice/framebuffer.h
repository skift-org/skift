#pragma once

/* Copyright © 2018-2019 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <libsystem/runtime.h>
#include <libgraphic/shape.h>

#define FRAMEBUFFER_DEVICE "/dev/fb"

#define FRAMEBUFFER_CALL_SET_MODE 0

#define FRAMEBUFFER_CALL_GET_MODE 1

typedef struct
{
    Point size;
} framebuffer_mode_arg_t;

// Blit a RGBA buffer to the screen.
#define FRAMEBUFFER_CALL_BLIT 2
typedef struct
{
    Point size;
    void *buffer;
} framebuffer_blit_args_t;

// Blit a region of an RGBA buffer to the screen.
#define FRAMEBUFFER_CALL_BLITREGION 3
typedef struct
{
    Point size;
    Rectangle region_to_blit;
    void *buffer;
} framebuffer_blitregion_args_t;

#define FRAMEBUFFER_CHANNEL "#dev:framebuffer"
#define FRAMEBUFFER_MODE_CHANGE "framebuffer.modechange"
