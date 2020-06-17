#pragma once

#include <abi/Handle.h>

struct Stream
{
    Handle handle;

    StreamBufferMode read_mode;
    void *write_buffer;
    int write_used;

    StreamBufferMode write_mode;
    void *read_buffer;
    int read_used;
    int read_head;

    bool has_unget;
    int unget_char;
};
