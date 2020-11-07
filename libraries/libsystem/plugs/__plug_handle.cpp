#include <abi/Syscalls.h>

#include <libsystem/core/CString.h>
#include <libsystem/core/Plugs.h>

void __plug_handle_open(Handle *handle, const char *path, OpenFlag flags)
{
    handle->result = hj_handle_open(&handle->id, path, strnlen(path, PATH_LENGTH), flags);
}

void __plug_handle_close(Handle *handle)
{
    if (handle->id >= 0)
    {
        handle->result = hj_handle_close(handle->id);
    }
}

Result __plug_handle_poll(HandleSet *handles, int *selected, PollEvent *selected_events, Timeout timeout)
{
    return hj_handle_poll(handles, selected, selected_events, timeout);
}

size_t __plug_handle_read(Handle *handle, void *buffer, size_t size)
{
    size_t read = 0;

    handle->result = hj_handle_read(handle->id, buffer, size, &read);

    return read;
}

size_t __plug_handle_write(Handle *handle, const void *buffer, size_t size)
{
    size_t written = 0;

    handle->result = hj_handle_write(handle->id, buffer, size, &written);

    return written;
}

Result __plug_handle_call(Handle *handle, IOCall request, void *args)
{
    handle->result = hj_handle_call(handle->id, request, args);

    return handle->result;
}

int __plug_handle_seek(Handle *handle, int offset, Whence whence)
{
    handle->result = hj_handle_seek(handle->id, offset, whence);
    return 0;
}

int __plug_handle_tell(Handle *handle, Whence whence)
{
    int offset = 0;

    handle->result = hj_handle_tell(handle->id, whence, &offset);

    return offset;
}

int __plug_handle_stat(Handle *handle, FileState *stat)
{
    handle->result = hj_handle_stat(handle->id, stat);

    return 0;
}

void __plug_handle_connect(Handle *handle, const char *path)
{
    handle->result = hj_handle_connect(&handle->id, path, strnlen(path, PATH_LENGTH));
}

void __plug_handle_accept(Handle *handle, Handle *connection_handle)
{
    handle->result = hj_handle_accept(handle->id, &connection_handle->id);
}

Result __plug_create_pipe(int *reader_handle, int *writer_handle)
{
    return hj_create_pipe(reader_handle, writer_handle);
}

Result __plug_create_term(int *server_handle, int *client_handle)
{
    return hj_create_term(server_handle, client_handle);
}
