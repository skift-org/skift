
#include <abi/Syscalls.h>

#include <libsystem/core/Plugs.h>

void __plug_handle_open(Handle *handle, const char *path, OpenFlag flags)
{
    handle->result = __syscall(SYS_HANDLE_OPEN, (int)&handle->id, (int)path, (int)flags);
}

void __plug_handle_close(Handle *handle)
{
    if (handle->id >= 0)
    {
        handle->result = __syscall(SYS_HANDLE_CLOSE, handle->id);
    }
}

Result __plug_handle_select(HandleSet *handles, int *selected, SelectEvent *selected_events, Timeout timeout)
{
    return __syscall(SYS_HANDLE_SELECT, (int)handles, (int)selected, (int)selected_events, timeout);
}

size_t __plug_handle_read(Handle *handle, void *buffer, size_t size)
{
    size_t read;

    handle->result = __syscall(SYS_HANDLE_READ, handle->id, (int)buffer, size, (int)&read);

    return read;
}

size_t __plug_handle_write(Handle *handle, const void *buffer, size_t size)
{
    size_t written;

    handle->result = __syscall(SYS_HANDLE_WRITE, handle->id, (int)buffer, size, (int)&written);

    return written;
}

Result __plug_handle_call(Handle *handle, IOCall request, void *args)
{

    handle->result = __syscall(SYS_HANDLE_CALL, handle->id, request, (int)args);

    return handle->result;
}

int __plug_handle_seek(Handle *handle, int offset, Whence whence)
{
    handle->result = __syscall(SYS_HANDLE_SEEK, handle->id, offset, whence);

    return 0;
}

int __plug_handle_tell(Handle *handle, Whence whence)
{
    int offset;

    handle->result = __syscall(SYS_HANDLE_TELL, handle->id, whence, (int)&offset);

    return offset;
}

int __plug_handle_stat(Handle *handle, FileState *stat)
{
    handle->result = __syscall(SYS_HANDLE_STAT, handle->id, (int)stat);

    return 0;
}

void __plug_handle_connect(Handle *handle, const char *path)
{
    handle->result = __syscall(SYS_HANDLE_CONNECT, (int)&handle->id, (int)path);
}

void __plug_handle_accept(Handle *handle, Handle *connection_handle)
{
    handle->result = __syscall(SYS_HANDLE_ACCEPT, handle->id, (int)&connection_handle->id);
}

Result __plug_create_pipe(int *reader_handle, int *writer_handle)
{
    return __syscall(SYS_CREATE_PIPE, (int)reader_handle, (int)writer_handle);
}

Result __plug_create_term(int *master_handle, int *slave_handle)
{
    return __syscall(SYS_CREATE_TERM, (int)master_handle, (int)slave_handle);
}
