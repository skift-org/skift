#pragma once

#include "kernel/node/Node.h"

class FsFile : public FsNode
{
private:
    char *_buffer;
    size_t _buffer_allocated;
    size_t _buffer_size;

public:
    FsFile();

    ~FsFile() override;

    Result open(FsHandle &handle) override;

    size_t size() override;

    ResultOr<size_t> read(FsHandle &handle, void *buffer, size_t size) override;

    ResultOr<size_t> write(FsHandle &handle, const void *buffer, size_t size) override;
};
