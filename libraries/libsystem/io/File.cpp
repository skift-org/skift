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

ResultOr<Slice> file_read_all(String path)
{
    void *buff = nullptr;
    size_t size = 0;

    auto res = file_read_all(path.cstring(), &buff, &size);

    if (res != SUCCESS)
    {
        return res;
    }

    return Slice{make<SliceStorage>(SliceStorage::ADOPT, buff, size)};
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

Result file_copy(const char *src, const char *dst)
{
    __cleanup(stream_cleanup) Stream *streamin = stream_open(src, OPEN_READ);

    if (handle_has_error(streamin))
    {
        return handle_get_error(streamin);
    }

    __cleanup(stream_cleanup) Stream *streamout = stream_open(dst, OPEN_WRITE | OPEN_CREATE);

    if (handle_has_error(streamout))
    {
        return handle_get_error(streamout);
    }

    size_t read;
    char buffer[1024];

    while ((read = stream_read(streamin, &buffer, 1024)) != 0)
    {
        if (handle_has_error(streamin))
        {
            return handle_get_error(streamin);
        }

        stream_write(streamout, buffer, read);

        if (handle_has_error(streamout))
        {
            return handle_get_error(streamout);
        }
    }

    return SUCCESS;
}
