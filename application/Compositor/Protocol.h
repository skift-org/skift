#pragma once

#include <libgraphic/Shape.h>
#include <libwidget/core/Cursor.h>
#include <libwidget/core/Event.h>

typedef enum
{
    COMPOSITOR_MESSAGE_INVALID,
    COMPOSITOR_MESSAGE_CREATE_WINDOW,
    COMPOSITOR_MESSAGE_DESTROY_WINDOW,
    COMPOSITOR_MESSAGE_BLIT_WINDOW,
    COMPOSITOR_MESSAGE_WINDOW_EVENT,
    COMPOSITOR_MESSAGE_WINDOW_MOVE,
    COMPOSITOR_MESSAGE_CURSOR_STATE_CHANGE,
} CompositorMessageType;

typedef struct
{
    CompositorMessageType type;
    size_t size;
} CompositorMessage;

typedef struct
{
    int id;
    int framebuffer;
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
} CompositorDestroyWindowMessage;

typedef struct
{
    int id;
    Rectangle bound;
} CompositorBlitWindowMessage;

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
