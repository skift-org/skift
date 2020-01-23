#include <libkernel/syscalls.h>
#include <libsystem/__plugs__.h>

void __plug_handle_open(Handle *handle, const char *path, OpenFlag flags)
{
    handle->error = __syscall(SYS_HANDLE_OPEN, (int)&handle->id, (int)path, (int)flags, 0, 0);
}

void __plug_handle_close(Handle *handle)
{
    handle->error = __syscall(SYS_HANDLE_CLOSE, handle->id, 0, 0, 0, 0);
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