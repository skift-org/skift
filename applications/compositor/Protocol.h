#pragma once

#include <libgraphic/Shape.h>
#include <libwidget/Cursor.h>
#include <libwidget/Event.h>

typedef enum
{
    COMPOSITOR_MESSAGE_INVALID,
    COMPOSITOR_MESSAGE_ACK,

    COMPOSITOR_MESSAGE_CREATE_WINDOW,
    COMPOSITOR_MESSAGE_DESTROY_WINDOW,
    COMPOSITOR_MESSAGE_FLIP_WINDOW,
    COMPOSITOR_MESSAGE_CURSOR_STATE_CHANGE,
    COMPOSITOR_MESSAGE_WINDOW_MOVE,
    COMPOSITOR_MESSAGE_WINDOW_RESIZE,

    COMPOSITOR_MESSAGE_WINDOW_EVENT,
} CompositorMessageType;

typedef struct
{
    CompositorMessageType type;
    size_t size;
} CompositorMessage;

typedef struct
{
    int id;
    int frontbuffer;
    int backbuffer;
    Rectangle bound;
} CompositorCreateWindowMessage;

typedef struct
{
    int id;
    Point position;
} CompositorWindowMove;

typedef struct
{
    int id;
    Rectangle bound;
} CompositorWindowResize;

typedef struct
{
    int id;
} CompositorDestroyWindowMessage;

typedef struct
{
    int id;
    int frontbuffer;
    int backbuffer;

    Rectangle bound;
} CompositorFlipWindowMessage;

typedef struct
{
    int id;
    Event event[];
} CompositorWindowEvent;

typedef struct
{
    int id;
    CursorState state;
} CompositorCursorStateChange;
