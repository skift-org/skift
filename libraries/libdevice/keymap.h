#pragma once

#include <libdevice/keys.h>

typedef struct 
{
    key_t key;

    int regular_codepoint;
    int shift_codepoint;
    int alt_codepoint;
    int shift_alt_codepoint;
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

