#pragma once

#include <abi/Keyboard.h>

#include <libmath/Rect.h>
#include <libtext/Rune.h>
#include <libutils/Func.h>

namespace Widget
{

#define MOUSE_NO_BUTTON (0)
#define MOUSE_BUTTON_LEFT (1 << 1)
#define MOUSE_BUTTON_RIGHT (1 << 2)
#define MOUSE_BUTTON_MIDDLE (1 << 3)

typedef unsigned int MouseButton;

struct MouseEvent
{
    int scroll;

    Math::Vec2i position;
    Math::Vec2i old_position;

    Math::Vec2i position_on_screen;
    Math::Vec2i old_position_on_screen;

    MouseButton button;
    MouseButton buttons;
};

struct KeyboardEvent
{
    Key key;
    KeyModifier modifiers;
    Text::Rune rune;
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
using EventHandler = Func<void(Event *)>;

#define is_mouse_event(__event)                                                       \
    (((::Widget::Event *)(__event))->type == ::Widget::Event::MOUSE_MOVE ||           \
     ((::Widget::Event *)(__event))->type == ::Widget::Event::MOUSE_SCROLL ||         \
     ((::Widget::Event *)(__event))->type == ::Widget::Event::MOUSE_ENTER ||          \
     ((::Widget::Event *)(__event))->type == ::Widget::Event::MOUSE_LEAVE ||          \
     ((::Widget::Event *)(__event))->type == ::Widget::Event::MOUSE_BUTTON_PRESS ||   \
     ((::Widget::Event *)(__event))->type == ::Widget::Event::MOUSE_BUTTON_RELEASE || \
     ((::Widget::Event *)(__event))->type == ::Widget::Event::MOUSE_DOUBLE_CLICK)

} // namespace Widget
