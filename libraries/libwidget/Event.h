#pragma once

#include <abi/Keyboard.h>

#include <libutils/Rect.h>

#include <libutils/unicode/Codepoint.h>
#include <libutils/Callback.h>

#define MOUSE_NO_BUTTON (0)
#define MOUSE_BUTTON_LEFT (1 << 1)
#define MOUSE_BUTTON_RIGHT (1 << 2)
#define MOUSE_BUTTON_MIDDLE (1 << 3)

typedef unsigned int MouseButton;

struct MouseEvent
{
    int scroll;

    Vec2i position;
    Vec2i old_position;

    Vec2i position_on_screen;
    Vec2i old_position_on_screen;

    MouseButton button;
    MouseButton buttons;
};

struct KeyboardEvent
{
    Key key;
    KeyModifier modifiers;
    Codepoint codepoint;
};

struct Event
{
    enum Type
    {
        VALUE_CHANGE,
        ACTION,

        GOT_FOCUS,
        LOST_FOCUS,

        WINDOW_CLOSING,
        WINDOW_RESIZED,

        WIDGET_DISABLE,
        WIDGET_ENABLE,

        MOUSE_MOVE,
        MOUSE_SCROLL,
        MOUSE_ENTER,
        MOUSE_LEAVE,

        MOUSE_BUTTON_PRESS,
        MOUSE_BUTTON_RELEASE,
        MOUSE_DOUBLE_CLICK,

        KEYBOARD_KEY_PRESS,
        KEYBOARD_KEY_RELEASE,
        KEYBOARD_KEY_TYPED,

        DISPLAY_SIZE_CHANGED,

        __COUNT,
    };

    Type type;
    bool accepted;

    MouseEvent mouse;
    KeyboardEvent keyboard;
};

using EventType = Event::Type;
using EventHandler = Callback<void(Event *)>;

#define is_mouse_event(__event)                                   \
    (((Event *)(__event))->type == Event::MOUSE_MOVE ||           \
     ((Event *)(__event))->type == Event::MOUSE_SCROLL ||         \
     ((Event *)(__event))->type == Event::MOUSE_ENTER ||          \
     ((Event *)(__event))->type == Event::MOUSE_LEAVE ||          \
     ((Event *)(__event))->type == Event::MOUSE_BUTTON_PRESS ||   \
     ((Event *)(__event))->type == Event::MOUSE_BUTTON_RELEASE || \
     ((Event *)(__event))->type == Event::MOUSE_DOUBLE_CLICK)
