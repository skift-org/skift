#include <libio/Streams.h>
#include <libsystem/io/Filesystem.h>

int main(int argc, char **argv)
{
    if (argc == 1)
    {
        IO::errln("{}: missing file operand.", argv[0]);
        return PROCESS_FAILURE;
    }

    if (argc == 2)
    {
        IO::errln("{}: missing destination file operand after '{}'.", argv[0], argv[1]);
        return PROCESS_FAILURE;
    }

    HjResult result = filesystem_rename(argv[1], argv[2]);

    if (result != SUCCESS)
    {
        IO::errln("{}: Failed to move file: {}.", argv[0], get_result_description(result));
        return PROCESS_FAILURE;
    }

    return PROCESS_SUCCESS;
}
