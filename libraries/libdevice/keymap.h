#pragma once

/* Copyright © 2018-2020 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <libdevice/keys.h>
#include <libsystem/unicode/Codepoint.h>

typedef struct
{
    Key key;

    Codepoint regular_codepoint;
    Codepoint shift_codepoint;
    Codepoint alt_codepoint;
    Codepoint shift_alt_codepoint;
} KeyMapping;

#define KEYMAP_LANGUAGE_SIZE 16
#define KEYMAP_REGION_SIZE 16

typedef struct
{
    char magic[4]; /* kmap */
    char language[KEYMAP_LANGUAGE_SIZE];
    char region[KEYMAP_REGION_SIZE];

    int mappings_count;
    KeyMapping mappings[];
} KeyMap;
