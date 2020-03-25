#pragma once

#include <abi/Message.h>

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
    Message header;
    CompositorMessageType type;
} CompositorMessage;

typedef struct
{
    CompositorMessage header;

    int id;
    int framebuffer;
    Rectangle bound;
} CompositorCreateWindowMessage;

typedef struct
{
    CompositorMessage header;
    int id;
} CompositorDestroyWindowMessage;

typedef struct
{
    CompositorMessage header;
    int id;
    Rectangle bound;
} CompositorBlitWindowMessage;

typedef struct
{
    CompositorMessage header;
    int id;
    Event event[];
} CompositorWindowEvent;
