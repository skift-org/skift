#pragma once

#include "kernel/node/Node.h"

class FsFile : public FsNode
{
private:
public:
    char *_buffer;
    size_t _buffer_allocated;
    size_t _buffer_size;

    FsFile();

    ResultOr<size_t> read(FsHandle &handle, void *buffer, size_t size);

    ResultOr<size_t> write(FsHandle &handle, const void *buffer, size_t size);
};
