#pragma once

#include <libsystem/io/Stream.h>

struct Pipe
{
    Stream *in;
    Stream *out;
};

Pipe *pipe_create();

void pipe_destroy(Pipe *pipe);
