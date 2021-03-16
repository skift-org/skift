#pragma once

#include <libio/Handle.h>
#include <libio/Reader.h>
#include <libio/Writer.h>
#include <libutils/Path.h>

namespace IO
{

class Connection final :
    public Reader,
    public Writer,
    public RawHandle
{
private:
    RefPtr<Handle> _handle;

    __nonmovable(Connection);

public:
    RefPtr<Handle> handle() override { return _handle; }

    Connection(RefPtr<Handle> handle)
        : _handle{handle} {}

    ResultOr<size_t> read(void *buffer, size_t size) override
    {
        return _handle->read(buffer, size);
    }

    ResultOr<size_t> write(const void *buffer, size_t size) override
    {
        return _handle->write(buffer, size);
    }
};

} // namespace IO