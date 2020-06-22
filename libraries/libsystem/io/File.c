#include <libsystem/io/File.h>
#include <libsystem/io/Stream.h>

Result file_read_all(const char *path, void **buffer, size_t *size)
{
    Stream *stream = stream_open(path, OPEN_READ);

    if (handle_has_error(stream))
    {
        return handle_get_error(stream);
    }

    FileState state = {};
    stream_stat(stream, &state);

    if (handle_has_error(stream))
    {
        return handle_get_error(stream);
    }

    *buffer = malloc(state.size);
    *size = state.size;

    stream_read(stream, *buffer, state.size);

    if (handle_has_error(stream))
    {
        free(buffer);
        return handle_get_error(stream);
    }

    return SUCCESS;
}
