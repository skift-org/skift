#include <abi/Syscalls.h>
#include <skift/Environment.h>

#include <libsystem/core/Plugs.h>
#include <libsystem/io/Filesystem.h>
#include <libsystem/io_new/Directory.h>
#include <libsystem/process/Process.h>
#include <libutils/Path.h>

Result __plug_process_get_directory(char *buffer, size_t size)
{
    auto &pwd = environment().get("POSIX").get("PWD");
    strncpy(buffer, pwd.as_string().cstring(), size);
    return SUCCESS;
}

Result __plug_process_set_directory(const char *path)
{
    auto new_path = process_resolve(path);

    System::Directory directory(new_path);

    if (!directory.exist())
    {
        return ERR_NO_SUCH_FILE_OR_DIRECTORY;
    }

    environment().get("POSIX").put("PWD", new_path);

    return SUCCESS;
}

String __plug_process_resolve(String raw_path)
{
    if (!environment().has("POSIX") ||
        !environment().get("POSIX").has("PWD") ||
        !environment().get("POSIX").get("PWD").is(json::STRING))
    {
        return raw_path;
    }

    auto &raw_pwd = environment().get("POSIX").get("PWD");

    auto path = Path::parse(raw_path);

    if (!path.absolute())
    {
        auto pwd = Path::parse(raw_pwd.as_string());

        path = Path::join(pwd, path);
    }

    return path.normalized().string();
}

Result filesystem_link(const char *raw_old_path, const char *raw_new_path)
{
    auto old_path = process_resolve(raw_old_path);
    auto new_path = process_resolve(raw_new_path);

    return hj_filesystem_link(
        old_path.cstring(), old_path.length(),
        new_path.cstring(), new_path.length());
}

Result filesystem_rename(const char *raw_old_path, const char *raw_new_path)
{
    auto old_path = process_resolve(raw_old_path);
    auto new_path = process_resolve(raw_new_path);

    return hj_filesystem_rename(
        old_path.cstring(), old_path.length(),
        new_path.cstring(), new_path.length());
}

Result filesystem_unlink(const char *raw_path)
{
    auto path = process_resolve(raw_path);

    return hj_filesystem_unlink(path.cstring(), path.length());
}

Result filesystem_mkdir(const char *raw_path)
{
    auto path = process_resolve(raw_path);

    return hj_filesystem_mkdir(path.cstring(), path.length());
}

Result filesystem_mkpipe(const char *raw_path)
{
    auto path = process_resolve(raw_path);

    return hj_filesystem_mkpipe(path.cstring(), path.length());
}

void __plug_handle_open(Handle *handle, const char *raw_path, OpenFlag flags)
{
    auto path = process_resolve(raw_path);

    handle->result = hj_handle_open(&handle->id, path.cstring(), path.length(), flags);
}

void __plug_handle_connect(Handle *handle, const char *raw_path)
{
    auto path = process_resolve(raw_path);

    handle->result = hj_handle_connect(&handle->id, path.cstring(), path.length());
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
