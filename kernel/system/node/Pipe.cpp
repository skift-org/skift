
#include <abi/Result.h>

#include "system/node/Handle.h"
#include "system/node/Pipe.h"

FsPipe::FsPipe() : FsNode(HJ_FILE_TYPE_PIPE)
{
}

bool FsPipe::can_read(FsHandle &)
{
    // FIXME: make this atomic or something...
    return !_buffer.empty() || !writers();
}

bool FsPipe::can_write(FsHandle &)
{
    // FIXME: make this atomic or something...
    return !_buffer.full() || !readers();
}

ResultOr<size_t> FsPipe::read(FsHandle &handle, void *buffer, size_t size)
{
    UNUSED(handle);

    if (!writers() && _buffer.empty())
    {
        return ERR_STREAM_CLOSED;
    }

    return _buffer.read((char *)buffer, size);
}

ResultOr<size_t> FsPipe::write(FsHandle &handle, const void *buffer, size_t size)
{
    UNUSED(handle);

    if (!readers())
    {
        return ERR_STREAM_CLOSED;
    }

    return _buffer.write((const char *)buffer, size);
}
