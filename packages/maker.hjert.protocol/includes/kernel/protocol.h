#pragma once

#include <skift/generic.h>
#include "kernel/shared/keyboard.h"

#define MSGPAYLOAD_SIZE 1024
#define MSGLABEL_SIZE 128

typedef struct 
{
    uint id;
    uint reply;
    char label[MSGLABEL_SIZE];

    uint flags;
    
    uint to;
    uint from;

    void * payload;
    uint size;
} message_t;

/* --- keyboard events ------------------------------------------------------ */

#define KEYBOARD_CHANNEL  "#dev:keyboard"

#define KEYBOARD_KEYPRESSED  "dev:keyboard.keypressed"
#define KEYBOARD_KEYRELEASED "dev:keyboard.keyreleased"
#define KEYBOARD_KEYTYPED    "dev:keyboard.keytyped"

typedef struct
{
    char c;
    keyboard_key_t key;
} keyboard_event_t;

/* --- Mouse events --------------------------------------------------------- */

#define MOUSE_CHANNEL    "#dev:mouse"

#define MOUSE_MOVE       "dev:mouse.move"
#define MOUSE_SCROLL     "dev:mouse.scroll"
#define MOUSE_BUTTONDOWN "dev:mouse.buttonpressed"
#define MOUSE_BUTTONUP   "dev:mouse.buttonreleased"

//XXX: stop using mouse_syscalls
typedef struct 
{
    int x;
    int y;
    int scroll;

    bool left;
    bool right;
    bool middle;
} mouse_state_t;

typedef enum
{
    MOUSE_BUTTON_LEFT,
    MOUSE_BUTTON_RIGHT,
    MOUSE_BUTTON_MIDDLE
} mouse_button_t;

typedef struct
{
    int off;
} mouse_scroll_event_t;

typedef struct
{
    int offx;
    int offy;
} mouse_move_event_t;

typedef struct
{
    mouse_button_t button;
} mouse_button_event_t;