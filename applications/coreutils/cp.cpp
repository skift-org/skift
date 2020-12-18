#include <libsystem/io/File.h>
#include <libsystem/io/Filesystem.h>
#include <libsystem/io/Stream.h>
#include <libutils/Path.h>

// implementation of cp open a input stream from file to copied and create a new file and open it as output stream and
// paste all the content into it.
// Add more options
// - Create new file
// - append to already existing file
// - only copy if file already exists
// - only logs if used with option -v

int main(int argc, char **argv)
{
    if (argc == 1)
    {
        stream_format(err_stream, "%s: Missing file operand\n", argv[0]);
        return PROCESS_FAILURE;
    }

    if (argc == 2)
    {
        stream_format(err_stream, "%s: Missing destination file operand after '%s'\n", argv[0], argv[1]);
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
