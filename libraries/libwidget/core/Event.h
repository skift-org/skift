#pragma once

#include <libdevice/keys.h>
#include <libgraphic/Shape.h>

#define EVENT_LIST(__EVENT) \
    __EVENT(CHILD_ADDED)

typedef enum
{
    EVENT_CHILD_ADDED,
    EVENT_CHILD_REMOVED,
    EVENT_PAINT,

    EVENT_MOUSE_MOVE,
    EVENT_MOUSE_ENTER,
    EVENT_MOUSE_LEAVE,
    EVENT_MOUSE_BUTTON_PRESS,
    EVENT_MOUSE_BUTTON_RELEASE,

    __EVENT_TYPE_COUNT,
} EventType;

typedef struct Event
{
    EventType type;
    bool accepted;
} Event;

typedef enum
{
    MOUSE_NO_BUTTON = 0,
    MOUSE_BUTTON_LEFT = 1 << 1,
    MOUSE_BUTTON_RIGHT = 1 << 2,
    MOUSE_BUTTON_MIDDLE = 1 << 3,
} MouseButton;

typedef struct
{
    Event event;
    Point position;
    Point old_position;

    MouseButton button;
    MouseButton buttons;
} MouseEvent;