#pragma once

#include <abi/Message.h>

#include <libgraphic/shape.h>

typedef enum
{
    COMPOSITOR_MESSAGE_CREATE_WINDOW,
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