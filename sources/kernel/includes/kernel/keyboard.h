#pragma once

#define KEYCOUNT 0

void keyboard_setup();

typedef struct 
{
    char regular;
    char shift;
    char ctrl;
} key_mapping_t;

typedef struct 
{
    const char * name;

} keymap_t;