
#include <libio/Streams.h>
#include <libsystem/io/Filesystem.h>

int main(int argc, char **argv)
{
    if (argc == 1)
    {
        IO::errln("{}: no eough arguments\n", argv[0]);
        return PROCESS_FAILURE;
    }

    return filesystem_link(argv[1], argv[2]);
}
