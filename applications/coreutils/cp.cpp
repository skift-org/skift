#include <libsystem/io/Filesystem.h>
#include <libsystem/io/Stream.h>

// implementation of cp open a input stream from file to copied and create a new file and open it as output stream and
// paste all the content into it.
// Add more options
// - Create new file
// - append to already existing file
// - only copy if file already exists
// - only logs if used with option -v

Result copy_file(Path *src, Path *dst)
{
    char *pathsrc = path_as_string(src);
    char *pathdst = path_as_string(dst);

    __cleanup(stream_cleanup) Stream *streamin = stream_open(pathsrc, OPEN_READ);

    if (handle_has_error(streamin))
    {
        handle_printf_error(streamin, "cp: Cannot access %s", pathsrc);
        return handle_get_error(streamin);
    }

    __cleanup(stream_cleanup) Stream *streamout = stream_open(pathdst, OPEN_WRITE | OPEN_CREATE);

    if (handle_has_error(streamout))
    {
        handle_printf_error(streamout, "cp: Cannot access %s", pathdst);
        return handle_get_error(streamout);
    }

    size_t read;
    char buffer[1024];

    while ((read = stream_read(streamin, &buffer, 1024)) != 0)
    {
        if (handle_has_error(streamin))
        {
            handle_printf_error(streamin, "cp: Failed to read from %s", pathsrc);
            return handle_get_error(streamin);
        }

        stream_write(streamout, buffer, read);

        if (handle_has_error(streamout))
        {
            handle_printf_error(streamout, "cp: Failed to write to %s", pathdst);
            return handle_get_error(streamout);
        }
    }
    stream_close(streamin);
    stream_close(streamout);

    free(pathsrc);
    free(pathdst);

    return SUCCESS;
}

int main(int argc, char **argv)
{
    if (argc == 1)
    {
        stream_format(err_stream, "%s: missing file operand\n", argv[0]);
        return PROCESS_FAILURE;
    }

    if (argc == 2)
    {
        stream_format(err_stream, "%s: missing destination file operand after '%s'\n", argv[0], argv[1]);
        return PROCESS_FAILURE;
    }

    Path *src = path_create(argv[1]);
    Path *dst = path_create(argv[2]);

    Result result = copy_file(src, dst);

    if (result != SUCCESS)
    {
        return PROCESS_FAILURE;
    }

    return PROCESS_SUCCESS;
}
