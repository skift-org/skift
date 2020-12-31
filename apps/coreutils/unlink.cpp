#include <libsystem/io/Filesystem.h>
#include <libsystem/io/Stream.h>

int main(int argc, char **argv)
{
    if (argc == 1)
    {
        stream_format(err_stream, "%s: missing operand", argv[0]);
        return PROCESS_FAILURE;
    }

    if (argc < 2)
    {
        stream_format(err_stream, "%s: extra operand\n,", argv[2]);
        return PROCESS_FAILURE;
    }

    return filesystem_unlink(argv[1]);
}
