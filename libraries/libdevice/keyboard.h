#pragma once

/* Copyright © 2018-2019 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <libdevice/keys.h>

#define KEYBOARD_DEVICE "/dev/kbd"

#define KEYBOARD_CHANNEL "#dev:keyboard"

#define KEYBOARD_KEYPRESSED "dev:keyboard.keypressed"
#define KEYBOARD_KEYRELEASED "dev:keyboard.keyreleased"
#define KEYBOARD_KEYTYPED "dev:keyboard.keytyped"

#define FRAMEBUFFER_CALL_SET_KEYMAP 0

typedef struct 
{
    uint size;
    void* keymap;
} keyboard_set_keymap_args_t;

#define FRAMEBUFFER_CALL_GET_KEYMAP 1

typedef struct
{
    key_t key;
    int codepoint;
} keyboard_event_t;

