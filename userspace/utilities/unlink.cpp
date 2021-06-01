#include <abi/Syscalls.h>

#include <libio/Streams.h>

int main(int argc, char **argv)
{
    if (argc == 1)
    {
        IO::errln("{}: missing operand", argv[0]);
        return PROCESS_FAILURE;
    }

    if (argc < 2)
    {
        IO::errln("{}: extra operand\n,", argv[2]);
        return PROCESS_FAILURE;
    }

    return hj_filesystem_unlink(argv[1], strlen(argv[1]));
}
