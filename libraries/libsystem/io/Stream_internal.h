#pragma once

#include <abi/Handle.h>
#include <libsystem/io/BufferMode.h>

struct Stream
{
    Handle handle;

    StreamBufferMode read_mode;
    void *write_buffer;
    size_t write_used;

    StreamBufferMode write_mode;
    void *read_buffer;
    size_t read_used;
    size_t read_head;

    bool has_unget;
    int unget_char;

    bool is_end_of_file;
};
