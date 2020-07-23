#include <libsystem/io/Filesystem.h>
#include <libsystem/io/Stream.h>

bool filesystem_exist(const char *path, FileType type)
{
    __cleanup(stream_cleanup) Stream *stream = stream_open(path, OPEN_READ);

    if (handle_has_error(stream))
    {
        return false;
    }

    FileState state = {};
    stream_stat(stream, &state);

    if (state.type != type)
    {
        return false;
    }

    return true;
}
