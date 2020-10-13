
#include <libsystem/io/Filesystem.h>
#include <libsystem/io/Stream.h>

int main(int argc, char **argv)
{
    if (argc == 1)
    {
        stream_format(err_stream, "%s: missing file operand\n", argv[0]);
        return PROCESS_FAILURE;
    }

    if (argc == 2)
    {
        stream_format(err_stream, "%s: missing destination file operand after '%s'", argv[0], argv[1]);
        return PROCESS_FAILURE;
    }

    Result result = filesystem_rename(argv[1], argv[2]);

    if (result != SUCCESS)
    {
        stream_format(err_stream, "Failed to move file: %s", get_result_description(result));
        return PROCESS_FAILURE;
    }

    return PROCESS_SUCCESS;
}
