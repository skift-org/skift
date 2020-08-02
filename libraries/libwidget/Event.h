#pragma once

#include <abi/Keyboard.h>
#include <libgraphic/Shape.h>
#include <libsystem/unicode/Codepoint.h>

#define EVENT_LIST(__EVENT) \
    __EVENT(CHILD_ADDED)

enum EventType
{
    EVENT_VALUE_CHANGE,
    EVENT_ACTION,

    EVENT_GOT_FOCUS,
    EVENT_LOST_FOCUS,

    EVENT_WINDOW_CLOSING,

    EVENT_MOUSE_MOVE,
    EVENT_MOUSE_ENTER,
    EVENT_MOUSE_LEAVE,

    EVENT_MOUSE_BUTTON_PRESS,
    EVENT_MOUSE_BUTTON_RELEASE,
    EVENT_MOUSE_DOUBLE_CLICK,

    EVENT_KEYBOARD_KEY_PRESS,
    EVENT_KEYBOARD_KEY_RELEASE,
    EVENT_KEYBOARD_KEY_TYPED,

    __EVENT_TYPE_COUNT,
};

#define MOUSE_NO_BUTTON (0)
#define MOUSE_BUTTON_LEFT (1 << 1)
#define MOUSE_BUTTON_RIGHT (1 << 2)
#define MOUSE_BUTTON_MIDDLE (1 << 3)

typedef unsigned int MouseButton;

struct MouseEvent
{
    Vec2i position;
    Vec2i old_position;

    MouseButton button;
    MouseButton buttons;
};

struct KeyboardEvent
{
    Key key;
    Codepoint codepoint;
};

struct Event
{
    EventType type;
    bool accepted;

    MouseEvent mouse;
    KeyboardEvent keyboard;
};

#define is_mouse_event(__event)                                  \
    (((Event *)(__event))->type == EVENT_MOUSE_MOVE ||           \
     ((Event *)(__event))->type == EVENT_MOUSE_ENTER ||          \
     ((Event *)(__event))->type == EVENT_MOUSE_LEAVE ||          \
     ((Event *)(__event))->type == EVENT_MOUSE_BUTTON_PRESS ||   \
     ((Event *)(__event))->type == EVENT_MOUSE_BUTTON_RELEASE || \
     ((Event *)(__event))->type == EVENT_MOUSE_DOUBLE_CLICK)

typedef void (*EventHandlerCallback)(void *target, void *sender, Event *event);

struct EventHandler
{
    void *target;
    EventHandlerCallback callback;
};

#define EVENT_HANDLER(__target, __callback) \
    ((EventHandler){(__target), (EventHandlerCallback)(__callback)})
