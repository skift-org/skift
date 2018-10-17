#pragma once

#include <skift/types.h>
#include <skift/utils.h>

typedef struct 
{
    uint width;
    uint height;
    
    bitmap_t * screen;
} hideo_context_t;

typedef PACKED(struct)
{
    int x;
    int y;
    
    uint width;
    uint height;

    char * title;
} hideo_window_t;

typedef enum
{
    BTN_PRESSED,
    BTN_RELEASED,
    BTN_UP,
    BTN_DOWN,
} hideo_button_state_t;

typedef PACKED(struct)
{    
    int x;
    int y;
    
    hideo_button_state_t leftbtn;
    hideo_button_state_t rightbtn;
    hideo_button_state_t middlebtn;
} hideo_cursor_t;

typedef PACKED(struct)
{

} hideo_keyboard_t;

