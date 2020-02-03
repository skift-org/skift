#include <abi/Syscalls.h>
#include <libsystem/__plugs__.h>

void __plug_handle_open(Handle *handle, const char *path, OpenFlag flags)
{
    handle->error = __syscall(SYS_HANDLE_OPEN, (int)&handle->id, (int)path, (int)flags, 0, 0);
}

void __plug_handle_close(Handle *handle)
{
    if (handle->id >= 0)
    {
        handle->error = __syscall(SYS_HANDLE_CLOSE, handle->id, 0, 0, 0, 0);
    }
}

error_t __plug_handle_select(int *handles, SelectEvent *events, size_t count, int *selected)
{
    return __syscall(SYS_HANDLE_SELECT, (int)handles, (int)events, count, (int)selected, 0);
}

size_t __plug_handle_read(Handle *handle, void *buffer, size_t size)
{
    size_t readed;

    handle->error = __syscall(SYS_HANDLE_READ, handle->id, (int)buffer, size, (int)&readed, 0);

    return readed;
}

size_t __plug_handle_write(Handle *handle, const void *buffer, size_t size)
{
    size_t writed;

    handle->error = __syscall(SYS_HANDLE_WRITE, handle->id, (int)buffer, size, (int)&writed, 0);

    return writed;
}

error_t __plug_handle_call(Handle *handle, int request, void *args)
{

    handle->error = __syscall(SYS_HANDLE_CALL, handle->id, request, (int)args, 0, 0);

    return handle->error;
}

int __plug_handle_seek(Handle *handle, int offset, Whence whence)
{
    handle->error = __syscall(SYS_HANDLE_SEEK, handle->id, offset, whence, 0, 0);

    return 0;
}

int __plug_handle_tell(Handle *handle, Whence whence)
{
    int offset;

    handle->error = __syscall(SYS_HANDLE_TELL, handle->id, whence, (int)&offset, 0, 0);

    return offset;
}

int __plug_handle_stat(Handle *handle, FileState *stat)
{
    handle->error = __syscall(SYS_HANDLE_STAT, handle->id, (int)stat, 0, 0, 0);

    return 0;
}

void __plug_handle_connect(Handle *handle, const char *path)
{
    handle->error = __syscall(SYS_HANDLE_CONNECT, (int)&handle->id, (int)path, 0, 0, 0);
}

void __plug_handle_accept(Handle *handle, Handle *connection_handle)
{
    handle->error = __syscall(SYS_HANDLE_ACCEPT, handle->id, (int)&connection_handle->id, 0, 0, 0);
}

void __plug_handle_send(Handle *handle, Message *message)
{
    handle->error = __syscall(SYS_HANDLE_SEND, handle->id, (int)message, 0, 0, 0);
}

void __plug_handle_receive(Handle *handle, Message *message)
{
    handle->error = __syscall(SYS_HANDLE_RECEIVE, handle->id, (int)message, 0, 0, 0);
}

void __plug_handle_payload(Handle *handle, Message *message)
{
    handle->error = __syscall(SYS_HANDLE_PAYLOAD, handle->id, (int)message, 0, 0, 0);
}

void __plug_handle_discard(Handle *handle)
{
    handle->error = __syscall(SYS_HANDLE_RECEIVE, handle->id, 0, 0, 0, 0);
}