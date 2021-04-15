#pragma once

#include <libio/Handle.h>
#include <libio/Path.h>
#include <libio/Reader.h>
#include <libio/Writer.h>

namespace IO
{

class Connection final :
    public Reader,
    public Writer,
    public RawHandle
{
private:
    RefPtr<Handle> _handle;

public:
    RefPtr<Handle> handle() override { return _handle; }

    Connection() {}
    Connection(RefPtr<Handle> handle) : _handle{handle} {}

    ResultOr<size_t> read(void *buffer, size_t size) override
    {
        if (!_handle)
            return ERR_STREAM_CLOSED;

        return _handle->read(buffer, size);
    }

    ResultOr<size_t> write(const void *buffer, size_t size) override
    {
        if (!_handle)
            return ERR_STREAM_CLOSED;

        return _handle->write(buffer, size);
    }

    bool closed()
    {
        return _handle == nullptr;
    }

    void close()
    {
        _handle = nullptr;
    }
};

} // namespace IO