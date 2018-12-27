#pragma once

#include <skift/generic.h>

#define MSGPAYLOAD_SIZE 1024
#define MSGNAME_SIZE 128

typedef struct 
{
    uint id;
    uint reply;
    char name[MSGNAME_SIZE];

    uint flags;
    
    uint to;
    uint from;

    void * payload;
    uint size;
} message_t;

/* --- keyboard events ------------------------------------------------------ */

#define KEYBOARD_CHANNEL  "#dev:keyboard"

#define KEYBOARD_KEYPRESS "dev:keyboard.keydown"
#define KEYBOARD_KEYUP    "dev:keyboard.keyup"
#define KEYBOARD_KEYTYPED "dev:keyboard.keytyped"

typedef struct
{
    char c;

    bool ctrl;
    bool altgr;
    bool shift;
} keyboard_event_t;

/* --- Mouse events --------------------------------------------------------- */

#define MOUSE_CHANNEL "#dev:mouse"

#define MOUSE_MOVE    "dev:mouse.move"
#define MOUSE_SCROLL  "dev:mouse.scroll"
#define MOUSE_DOWN    "dev:mouse.down"
#define MOUSE_UP      "dev:mouse.up"
#define MOUSE_CLICK   "dev:mouse.click"

typedef enum
{
    MOUSE_BUTTON_LEFT,
    MOUSE_BUTTON_RIGHT,
    MOUSE_BUTTON_CENTER
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