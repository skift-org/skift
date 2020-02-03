#include <libsystem/assert.h>

#include "node/Handle.h"
#include "node/Terminal.h"

#include <libsystem/logger.h>

#define TERMINAL_RINGBUFFER_SIZE 1024

bool terminal_FsOperationCanRead(FsTerminal *terminal, FsHandle *handle)
{
    __unused(handle);

    if (fshandle_has_flag(handle, OPEN_MASTER))
    {
        return !ringbuffer_is_empty(terminal->slave_to_master_buffer);
    }
    else
    {
        return !ringbuffer_is_empty(terminal->master_to_slave_buffer);
    }
}

bool terminal_FsOperationCanWrite(FsTerminal *terminal, FsHandle *handle)
{
    __unused(handle);

    if (fshandle_has_flag(handle, OPEN_MASTER))
    {
        return !ringbuffer_is_full(terminal->master_to_slave_buffer);
    }
    else
    {
        return !ringbuffer_is_full(terminal->slave_to_master_buffer);
    }
}

error_t terminal_FsOperationRead(FsTerminal *terminal, FsHandle *handle, void *buffer, size_t size, size_t *readed)
{
    __unused(handle);

    if (fshandle_has_flag(handle, OPEN_MASTER))
    {
        *readed = ringbuffer_read(terminal->slave_to_master_buffer, buffer, size);
    }
    else
    {
        *readed = ringbuffer_read(terminal->master_to_slave_buffer, buffer, size);
    }

    return ERR_SUCCESS;
}

error_t terminal_FsOperationWrite(FsTerminal *terminal, FsHandle *handle, const void *buffer, size_t size, size_t *writen)
{
    __unused(handle);

    if (fshandle_has_flag(handle, OPEN_MASTER))
    {
        *writen = ringbuffer_write(terminal->master_to_slave_buffer, buffer, size);
    }
    else
    {
        *writen = ringbuffer_write(terminal->slave_to_master_buffer, buffer, size);
    }

    return ERR_SUCCESS;
}

size_t terminal_FsOperationSize(FsTerminal *terminal, FsHandle *handle)
{
    __unused(handle);
    __unused(terminal);

    return TERMINAL_RINGBUFFER_SIZE;
}

void terminal_FsOperationDestroy(FsTerminal *terminal)
{
    ringbuffer_destroy(terminal->master_to_slave_buffer);
    ringbuffer_destroy(terminal->slave_to_master_buffer);
}

FsNode *terminal_create(void)
{
    FsTerminal *terminal = __create(FsTerminal);

    fsnode_init(FSNODE(terminal), FSNODE_TERMINAL);

    FSNODE(terminal)->can_read = (FsOperationCanRead)terminal_FsOperationCanRead;
    FSNODE(terminal)->can_write = (FsOperationCanWrite)terminal_FsOperationCanWrite;
    FSNODE(terminal)->read = (FsOperationRead)terminal_FsOperationRead;
    FSNODE(terminal)->write = (FsOperationWrite)terminal_FsOperationWrite;
    FSNODE(terminal)->size = (FsOperationSize)terminal_FsOperationSize;
    FSNODE(terminal)->destroy = (FsOperationDestroy)terminal_FsOperationDestroy;

    terminal->master_to_slave_buffer = ringbuffer_create(TERMINAL_RINGBUFFER_SIZE);
    terminal->slave_to_master_buffer = ringbuffer_create(TERMINAL_RINGBUFFER_SIZE);

    return FSNODE(terminal);
}
