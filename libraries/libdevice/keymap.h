#pragma once

#include <libdevice/keys.h>
#include <libsystem/unicode/Codepoint.h>

typedef struct
{
    key_t key;

    Codepoint regular_codepoint;
    Codepoint shift_codepoint;
    Codepoint alt_codepoint;
    Codepoint shift_alt_codepoint;
} keymap_keybing_t;

#define KEYMAP_LANGUAGE_SIZE 16
#define KEYMAP_REGION_SIZE 16

typedef struct
{
    char magic[4]; /* kmap */
    char language[KEYMAP_LANGUAGE_SIZE];
    char region[KEYMAP_REGION_SIZE];

    int bindings_count;
    keymap_keybing_t bindings[];
} keymap_t;
