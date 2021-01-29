#include <libsystem/io/File.h>
#include <libsystem/io/Stream.h>

Result File::read_all(void **buffer, size_t *size)
{
    __cleanup(stream_cleanup) Stream *stream = stream_open(_path.string().cstring(), OPEN_READ);

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
        free(*buffer);
        return handle_get_error(stream);
    }

    return SUCCESS;
}

ResultOr<Slice> File::read_all()
{
    void *buff = nullptr;
    size_t size = 0;

    auto res = read_all(&buff, &size);

    if (res == SUCCESS)
    {
        return Slice{make<SliceStorage>(SliceStorage::ADOPT, buff, size)};
    }
    else
    {
        return res;
    }
}

Result File::write_all(const void *buffer, size_t size)
{
    __cleanup(stream_cleanup) Stream *stream = stream_open(_path.string().cstring(), OPEN_WRITE | OPEN_CREATE);

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

bool File::exist()
{
    __cleanup(stream_cleanup) Stream *stream = stream_open(_path.string().cstring(), OPEN_READ);

    if (handle_has_error(stream))
    {
        return false;
    }

    FileState state = {};
    stream_stat(stream, &state);

    return state.type == FILE_TYPE_REGULAR;
}

Result File::copy(const char *destination)
{
    __cleanup(stream_cleanup) Stream *streamin = stream_open(_path.string().cstring(), OPEN_READ);

    if (handle_has_error(streamin))
    {
        return handle_get_error(streamin);
    }

    __cleanup(stream_cleanup) Stream *streamout = stream_open(destination, OPEN_WRITE | OPEN_CREATE);

    if (handle_has_error(streamout))
    {
        return handle_get_error(streamout);
    }

    size_t read;
    char buffer[4096];
    while ((read = stream_read(streamin, &buffer, 4096)) != 0)
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
