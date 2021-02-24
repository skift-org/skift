#pragma once

#include <abi/Handle.h>

#include "kernel/node/Node.h"

class FsHandle
{
private:
    Lock _lock{"fshandle"};
    RefPtr<FsNode> _node = nullptr;
    OpenFlag _flags = 0;
    size_t _offset = 0;

public:
    void *attached;
    size_t attached_size;

    auto node() { return _node; }

    auto offset() { return _offset; }

    auto flags() { return _flags; }

    bool has_flag(OpenFlag flag) { return (_flags & flag) == flag; }

    FsHandle(RefPtr<FsNode> node, OpenFlag flags);

    FsHandle(FsHandle &other);

    ~FsHandle();

    bool locked();

    void acquire(int who_acquire);

    void release(int who_release);

    PollEvent poll(PollEvent events);

    ResultOr<size_t> read(void *buffer, size_t size);

    ResultOr<size_t> write(const void *buffer, size_t size);

    ResultOr<int> seek(int offset, Whence whence);

    Result call(IOCall request, void *args);

    Result stat(FileState *stat);

    ResultOr<FsHandle *> accept();
};
