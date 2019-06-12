#pragma once

#define MOUSE_DEVICE     "/dev/mice"

#define MOUSE_CHANNEL    "#dev:mouse"

#define MOUSE_MOVE       "devmouse.move"
#define MOUSE_SCROLL     "devmouse.scroll"
#define MOUSE_BUTTONDOWN "devmouse.buttonpressed"
#define MOUSE_BUTTONUP   "devmouse.buttonreleased"

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