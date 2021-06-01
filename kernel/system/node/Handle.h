#pragma once

#include <abi/Handle.h>
#include <libio/Seek.h>

#include "system/node/Node.h"

struct FsHandle : public RefCounted<FsHandle>
{
private:
    Lock _lock{"fshandle"};
    RefPtr<FsNode> _node = nullptr;
    HjOpenFlag _flags = 0;
    size_t _offset = 0;

public:
    void *attached;
    size_t attached_size;

    auto node() { return _node; }

    auto offset() { return _offset; }

    auto flags() { return _flags; }

    bool has_flag(HjOpenFlag flag) { return (_flags & flag) == flag; }

    FsHandle(RefPtr<FsNode> node, HjOpenFlag flags);

    FsHandle(FsHandle &other);

    ~FsHandle();

    bool locked();

    void acquire(int who_acquire);

    void release(int who_release);

    PollEvent poll(PollEvent events);

    ResultOr<size_t> read(void *buffer, size_t size);

    ResultOr<size_t> write(const void *buffer, size_t size);

    ResultOr<ssize64_t> seek(IO::SeekFrom from);

    HjResult call(IOCall request, void *args);

    HjResult stat(HjStat *stat);

    ResultOr<RefPtr<FsHandle>> accept();
};
