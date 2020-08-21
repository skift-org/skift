
#include <libsystem/Result.h>
#include <libsystem/io/Stream.h>

int cat(const char *path)
{
    __cleanup(stream_cleanup) Stream *stream = stream_open(path, OPEN_READ);

    if (handle_has_error(stream))
    {
        handle_printf_error(stream, "cat: Cannot access %s", path);
        return -1;
    }

    FileState stat = {};
    stream_stat(stream, &stat);

    size_t read;
    char buffer[1024];

    while ((read = stream_read(stream, &buffer, 1024)) != 0)
    {
        if (handle_has_error(stream))
        {
            handle_printf_error(stream, "cat: Failed to read from %s", path);

            return -1;
        }

        stream_write(out_stream, buffer, read);

        if (handle_has_error(out_stream))
        {
            handle_printf_error(out_stream, "cat: Failed to write to stdout");

            return -1;
        }
    }

    stream_flush(out_stream);

    return 0;
}

int main(int argc, char **argv)
{
    if (argc >= 2)
    {
        int result = 0;

        for (int i = 1; i < argc; i++)
        {
            result |= cat(argv[i]);
        }

        return result;
    }

    return -1;
}
