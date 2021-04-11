#include <libsystem/io/Filesystem.h>

#include <libsystem/core/Plugs.h>

bool filesystem_exist(const char *path, FileType type)
{
    Handle handle;
    __plug_handle_open(&handle, path, 0);

    if (handle_has_error(&handle))
    {
        return false;
    }

    FileState state;
    __plug_handle_stat(&handle, &state);

    if (handle_has_error(&handle))
    {
        __plug_handle_close(&handle);
        return false;
    }

    if (state.type != type)
    {
        __plug_handle_close(&handle);
        return false;
    }

    __plug_handle_close(&handle);
    return true;
}
