#pragma once

#include <libutils/Rect.h>
#include <libwidget/Cursor.h>
#include <libwidget/Event.h>

enum CompositorMessageType
{
    COMPOSITOR_MESSAGE_INVALID,
    COMPOSITOR_MESSAGE_ACK,
    COMPOSITOR_MESSAGE_GREETINGS,
    COMPOSITOR_MESSAGE_GOODBYE,
    COMPOSITOR_MESSAGE_EVENT,
    COMPOSITOR_MESSAGE_CHANGED_RESOLUTION,

    COMPOSITOR_MESSAGE_CREATE_WINDOW,
    COMPOSITOR_MESSAGE_DESTROY_WINDOW,
    COMPOSITOR_MESSAGE_MOVE_WINDOW,
    COMPOSITOR_MESSAGE_FLIP_WINDOW,
    COMPOSITOR_MESSAGE_EVENT_WINDOW,
    COMPOSITOR_MESSAGE_CURSOR_WINDOW,
    COMPOSITOR_MESSAGE_SET_RESOLUTION,

    COMPOSITOR_MESSAGE_GET_MOUSE_POSITION,
    COMPOSITOR_MESSAGE_MOUSE_POSITION,
};

#define WINDOW_NONE (0)
#define WINDOW_BORDERLESS (1 << 0)
#define WINDOW_RESIZABLE (1 << 1)
#define WINDOW_ALWAYS_FOCUSED (1 << 2)
#define WINDOW_TRANSPARENT (1 << 4)
#define WINDOW_ACRYLIC (1 << 5)
#define WINDOW_NO_FOCUS (1 << 6)
#define WINDOW_AUTO_CLOSE (1 << 7)
#define WINDOW_NO_ROUNDED_CORNERS (1 << 8)

typedef unsigned int WindowFlag;

enum WindowType
{
    WINDOW_TYPE_POPOVER,
    WINDOW_TYPE_PANEL,
    WINDOW_TYPE_REGULAR,
    WINDOW_TYPE_DESKTOP,
};

struct CompositorGreetings
{
    Recti screen_bound;
};

struct CompositorEvent
{
    Widget::Event event;
};

struct CompositorCreateWindow
{
    int id;
    WindowFlag flags;
    WindowType type;

    int frontbuffer;
    Vec2i frontbuffer_size;
    int backbuffer;
    Vec2i backbuffer_size;

    Recti bound;
};

struct CompositorDestroyWindow
{
    int id;
};

struct CompositorMoveWindow
{
    int id;

    Vec2i position;
};

struct CompositorFlipWindow
{
    int id;

    int frontbuffer;
    Vec2i frontbuffer_size;
    int backbuffer;
    Vec2i backbuffer_size;

    Recti dirty;
    Recti bound;
};

struct CompositorEventWindow
{
    int id;

    Widget::Event event;
};

struct CompositorCursorWindow
{
    int id;

    Widget::CursorState state;
};

struct CompositorSetResolution
{
    int width;
    int height;
};

struct CompositorChangedResolution
{
    Recti resolution;
};

struct CompositorMousePosition
{
    Vec2i position;
};

struct CompositorMessage
{
    CompositorMessageType type;

    union {
        CompositorGreetings greetings;
        CompositorEvent event;
        CompositorCreateWindow create_window;
        CompositorDestroyWindow destroy_window;
        CompositorMoveWindow move_window;
        CompositorFlipWindow flip_window;
        CompositorEventWindow event_window;
        CompositorCursorWindow cursor_window;
        CompositorSetResolution set_resolution;
        CompositorChangedResolution changed_resolution;

        CompositorMousePosition mouse_position;
    };
};
