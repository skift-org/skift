
#include <libsystem/io/File.h>
#include <libsystem/io/Filesystem.h>
#include <libsystem/io/Stream.h>
#include <libutils/Path.h>

int main(int argc, char **argv)
{
    if (argc == 1)
    {
        stream_format(err_stream, "%s: Missing archive operand\n", argv[0]);
        return PROCESS_FAILURE;
    }

    if (argc == 2)
    {
        stream_format(err_stream, "%s: Missing files to pack into archive '%s'\n", argv[0], argv[1]);
        return PROCESS_FAILURE;
    }

    File file{argv[1]};

    Result result = file.copy(argv[2]);

    if (result != SUCCESS)
    {
        stream_format(err_stream, "%s: Failed to copy '%s' to '%s': %s", argv[1], argv[2], get_result_description(result));
        return PROCESS_FAILURE;
    }

    return PROCESS_SUCCESS;
}
