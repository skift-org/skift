
#include <libraries/libsystem/io/Filesystem.h>
#include <libsystem/Result.h>
#include <libsystem/io/Stream.h>

int main(int argc, char **argv)
{
    if (argc == 1)
    {
        stream_format(err_stream, "%s: no eough arguments\n", argv[0]);
        return PROCESS_FAILURE;
    }

    return filesystem_link(argv[1], argv[2]);
}
