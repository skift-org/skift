#pragma once

#include <abi/Message.h>

#include <libgraphic/Shape.h>

typedef enum
{
    COMPOSITOR_MESSAGE_CREATE_WINDOW,
    COMPOSITOR_MESSAGE_DESTROY_WINDOW,
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