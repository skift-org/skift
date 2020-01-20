#include <libsystem/__plugs__.h>
#include <libkernel/syscalls.h>

void __plug_handle_open(Handle *handle, const char *path, OpenFlag flags)
{
    handle->error = __syscall(SYS_HANDLE_OPEN, &handle->id, path, flags, 0, 0);
}

void __plug_handle_close(Handle *handle)
{
    handle->error = __syscall(SYS_HANDLE_CLOSE, handle->id, 0, 0, 0, 0);
}

void __plug_handle_send(Handle *handle, Message *message)
{
    handle->error = __syscall(SYS_HANDLE_SEND, &handle->id, message, 0, 0, 0);
}

void __plug_handle_receive(Handle *handle, Message *message)
{
    handle->error = __syscall(SYS_HANDLE_RECEIVE, &handle->id, message, 0, 0, 0);
}

void __plug_handle_request(Handle *handle, Message *request, Message *respond)
{
    handle->error = __syscall(SYS_HANDLE_REQUEST, &handle->id, request, respond, 0, 0);
}

void __plug_handle_payload(Handle *handle, Message *request)
{
    handle->error = __syscall(SYS_HANDLE_PAYLOAD, &handle->id, request, 0, 0, 0);
}

void __plug_handle_discard(Handle *handle)
{
    handle->error = __syscall(SYS_HANDLE_RECEIVE, &handle->id, 0, 0, 0, 0);
}

void __plug_handle_respond(Handle *handle, Message *message)
{
    handle->error = __syscall(SYS_HANDLE_RESPOND, &handle->id, message, 0, 0, 0);
}