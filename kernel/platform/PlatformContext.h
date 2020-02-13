#pragma once

/* Copyright © 2018-2020 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

typedef struct
{
    char fpu_registers[512];
} PlatformContext;

PlatformContext *platform_context_create(void);

void platform_context_destroy(PlatformContext *context);

void platform_context_save(PlatformContext *context);

void platform_context_restore(PlatformContext *context);
