#pragma once

/* Copyright © 2018-2020 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <libdevice/keys.h>
#include <libsystem/unicode/Codepoint.h>

#define KEYBOARD_DEVICE "/dev/keyboard"

#define KEYBOARD_CHANNEL "#dev:keyboard"

#define KEYBOARD_KEYPRESSED MSGLABEL("keyboard", "key", "pressed")
#define KEYBOARD_KEYRELEASED MSGLABEL("keyboard", "key", "released")
#define KEYBOARD_KEYTYPED MSGLABEL("keyboard", "key", "typed")

#define KEYBOARD_CALL_SET_KEYMAP 0

typedef struct
{
    uint size;
    void *keymap;
} keyboard_set_keymap_args_t;

#define KEYBOARD_CALL_GET_KEYMAP 1

typedef struct
{
    Key key;
    Codepoint codepoint;
} keyboard_event_t;
