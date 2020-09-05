
#include <libsystem/Logger.h>
#include <libsystem/Result.h>
#include <libsystem/core/CString.h>
#include <libsystem/math/MinMax.h>

#include "kernel/node/File.h"
#include "kernel/node/Handle.h"

static Result file_open(FsFile *node, FsHandle *handle)
{
    if (handle->has_flag(OPEN_TRUNC))
    {
        free(node->_buffer);
        node->_buffer = (char *)malloc(512);
        node->_buffer_allocated = 512;
        node->_buffer_size = 0;
    }

    return SUCCESS;
}

ResultOr<size_t> FsFile::read(FsHandle &handle, void *buffer, size_t size)
{
    size_t read = 0;

    if (handle.offset <= _buffer_size)
    {
        read = MIN(_buffer_size - handle.offset, size);
        memcpy(buffer, (char *)_buffer + handle.offset, read);
    }

    return read;
}

ResultOr<size_t> FsFile::write(FsHandle &handle, const void *buffer, size_t size)
{
    if ((handle.offset + size) > _buffer_allocated)
    {
        _buffer = (char *)realloc(_buffer, handle.offset + size);
        _buffer_allocated = handle.offset + size;
    }

    _buffer_size = MAX(handle.offset + size, _buffer_size);
    memcpy((char *)(_buffer) + handle.offset, buffer, size);

    return size;
}
static size_t file_size(FsFile *node, FsHandle *handle)
{
    __unused(handle);

    return node->_buffer_size;
}

static void file_destroy(FsFile *node)
{
    free(node->_buffer);
}

FsFile::FsFile() : FsNode(FILE_TYPE_REGULAR)
{
    open = (FsNodeOpenCallback)file_open;
    size = (FsNodeSizeCallback)file_size;
    destroy = (FsNodeDestroyCallback)file_destroy;

    _buffer = (char *)malloc(512);
    _buffer_allocated = 512;
    _buffer_size = 0;
}
