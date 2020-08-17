#include <libsystem/io/File.h>
#include <libsystem/io/Stream.h>

Result file_read_all(const char *path, void **buffer, size_t *size)
{
    __cleanup(stream_cleanup) Stream *stream = stream_open(path, OPEN_READ);

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

Result file_write_all(const char *path, void *buffer, size_t size)
{
    __cleanup(stream_cleanup) Stream *stream = stream_open(path, OPEN_WRITE | OPEN_CREATE);

    if (handle_has_error(stream))
    {
        return handle_get_error(stream);
    }

    size_t remaining = size;

    while (remaining)
    {
        remaining -= stream_write(stream, ((char *)buffer + (size - remaining)), size);

        if (handle_has_error(stream))
        {
            return handle_get_error(stream);
        }
    }

    return SUCCESS;
}

bool file_exist(const char *path)
{
    __cleanup(stream_cleanup) Stream *stream = stream_open(path, OPEN_READ);

    if (handle_has_error(stream))
    {
        return false;
    }

    FileState state = {};
    stream_stat(stream, &state);

    if (state.type != FILE_TYPE_REGULAR)
    {
        return false;
    }

    return true;
}
