#pragma once

#include <libgraphic/Shape.h>
#include <libwidget/core/Event.h>

typedef enum
{
    COMPOSITOR_MESSAGE_INVALID,
    COMPOSITOR_MESSAGE_CREATE_WINDOW,
    COMPOSITOR_MESSAGE_DESTROY_WINDOW,
    COMPOSITOR_MESSAGE_BLIT_WINDOW,
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
    int framebuffer;
    Rectangle bound;
} CompositorCreateWindowMessage;

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
