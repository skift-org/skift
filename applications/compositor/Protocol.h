#pragma once

#include <libgraphic/Shape.h>
#include <libwidget/Cursor.h>
#include <libwidget/Event.h>

typedef enum
{
    COMPOSITOR_MESSAGE_INVALID,
    COMPOSITOR_MESSAGE_ACK,
    COMPOSITOR_MESSAGE_GREETINGS,

    COMPOSITOR_MESSAGE_CREATE_WINDOW,
    COMPOSITOR_MESSAGE_DESTROY_WINDOW,
    COMPOSITOR_MESSAGE_RESIZE_WINDOW,
    COMPOSITOR_MESSAGE_MOVE_WINDOW,
    COMPOSITOR_MESSAGE_FLIP_WINDOW,
    COMPOSITOR_MESSAGE_EVENT_WINDOW,
    COMPOSITOR_MESSAGE_CURSOR_WINDOW,
    COMPOSITOR_MESSAGE_SET_RESOLUTION,
} CompositorMessageType;

typedef struct
{
    Rectangle screen_bound;
} CompositorGreetings;

typedef struct
{
    int id;

    int frontbuffer;
    int backbuffer;
    Rectangle bound;
} CompositorCreateWindow;

typedef struct
{
    int id;
} CompositorDestroyWindow;

typedef struct
{
    int id;

    Rectangle bound;
} CompositorResizeWindow;

typedef struct
{
    int id;

    Vec2i position;
} CompositorMoveWindow;

typedef struct
{
    int id;

    int frontbuffer;
    int backbuffer;
    Rectangle bound;
} CompositorFlipWindow;

typedef struct
{
    int id;

    Event event;
} CompositorEventWindow;

typedef struct
{
    int id;

    CursorState state;
} CompositorCursorWindow;

typedef struct
{
    int width;
    int height;
} CompositorSetResolution;

typedef struct
{
    CompositorMessageType type;

    union {
        CompositorGreetings greetings;
        CompositorCreateWindow create_window;
        CompositorDestroyWindow destroy_window;
        CompositorResizeWindow resize_window;
        CompositorMoveWindow move_window;
        CompositorFlipWindow flip_window;
        CompositorEventWindow event_window;
        CompositorCursorWindow cursor_window;
        CompositorSetResolution set_resolution;
    };
} CompositorMessage;
