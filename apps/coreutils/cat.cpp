
#include <libsystem/Result.h>
#include <libsystem/io/Stream.h>

Result cat(const char *path)
{
    __cleanup(stream_cleanup) Stream *stream = stream_open(path, OPEN_READ);

    if (handle_has_error(stream))
    {
        return handle_get_error(stream);
    }

    FileState stat = {};
    stream_stat(stream, &stat);

    size_t read;
    char buffer[1024];

    while ((read = stream_read(stream, &buffer, 1024)) != 0)
    {
        if (handle_has_error(stream))
        {
            return handle_get_error(stream);
        }

        stream_write(out_stream, buffer, read);

        if (handle_has_error(out_stream))
        {
            return ERR_WRITE_STDOUT;
        }
    }

    stream_flush(out_stream);

    return SUCCESS;
}

int main(int argc, char **argv)
{
    if (argc == 1)
    {
        /* TODO: stdin option */
        return PROCESS_SUCCESS;
    }

    Result result;
    int exit_code = PROCESS_SUCCESS;

    for (int i = 1; i < argc; i++)
    {
        result = cat(argv[i]);

        if (result != SUCCESS)
        {
            stream_format(err_stream, "%s: %s: %s", argv[0], argv[i], get_result_description(result));
            exit_code = PROCESS_FAILURE;
        }
    }

    return exit_code;
}
