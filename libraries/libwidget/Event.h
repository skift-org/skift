#pragma once

#include <libdevice/keys.h>
#include <libgraphic/Shape.h>
#include <libsystem/unicode/Codepoint.h>

#define EVENT_LIST(__EVENT) \
    __EVENT(CHILD_ADDED)

typedef enum
{
    EVENT_CHILD_ADDED,
    EVENT_CHILD_REMOVED,
    EVENT_PAINT,
    EVENT_LAYOUT,
    EVENT_VALUE_CHANGE,

    EVENT_GOT_FOCUS,
    EVENT_LOST_FOCUS,

    EVENT_MOUSE_MOVE,
    EVENT_MOUSE_ENTER,
    EVENT_MOUSE_LEAVE,
    EVENT_MOUSE_BUTTON_PRESS,
    EVENT_MOUSE_BUTTON_RELEASE,
    EVENT_MOUSE_DOUBLE_CLICK,

    EVENT_KEYBOARD_KEY_TYPED,

    __EVENT_TYPE_COUNT,
} EventType;

typedef struct Event
{
    EventType type;
    bool accepted;
} Event;

#define is_mouse_event(__event)                                  \
    (((Event *)(__event))->type == EVENT_MOUSE_MOVE ||           \
     ((Event *)(__event))->type == EVENT_MOUSE_ENTER ||          \
     ((Event *)(__event))->type == EVENT_MOUSE_LEAVE ||          \
     ((Event *)(__event))->type == EVENT_MOUSE_BUTTON_PRESS ||   \
     ((Event *)(__event))->type == EVENT_MOUSE_BUTTON_RELEASE || \
     ((Event *)(__event))->type == EVENT_MOUSE_DOUBLE_CLICK)

#define EVENT(__type, __event_type, __args...) \
    (Event *)&(__type)                         \
    {                                          \
        {                                      \
            __event_type,                      \
            false,                             \
        },                                     \
            __args                             \
    }

#define EVENT_NO_ARGS(__event_type) \
    &(Event)                        \
    {                               \
        __event_type,               \
            false,                  \
    }

#define MOUSE_NO_BUTTON (0)
#define MOUSE_BUTTON_LEFT (1 << 1)
#define MOUSE_BUTTON_RIGHT (1 << 2)
#define MOUSE_BUTTON_MIDDLE (1 << 3)

typedef unsigned int MouseButton;

typedef struct
{
    Event event;
    Point position;
    Point old_position;

    MouseButton button;
    MouseButton buttons;
} MouseEvent;

typedef struct
{
    Event event;
    Codepoint codepoint;
} KeyboardEvent;