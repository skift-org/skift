#pragma once

#include <libsystem/io/Stream.h>

typedef struct
{
    Stream *in;
    Stream *out;
} Pipe;

Pipe *pipe_create(void);

void pipe_destroy(Pipe *pipe);
