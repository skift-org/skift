#include <libio/Copy.h>
#include <libio/File.h>
#include <libio/Streams.h>

int main(int argc, char **argv)
{
    if (argc == 1)
    {
        IO::errln("{}: Missing file operand", argv[0]);
        return PROCESS_FAILURE;
    }

    if (argc == 2)
    {
        IO::errln("{}: Missing destination file operand after '{}'", argv[0], argv[1]);
        return PROCESS_FAILURE;
    }

    IO::File source{argv[1], OPEN_READ};
    IO::File destination{argv[2], OPEN_WRITE | OPEN_CREATE};

    Result result = IO::copy(source, destination);

    if (result != SUCCESS)
    {
        IO::errln("{}: Failed to copy '{}' to '{}': {}", argv[0], argv[1], argv[2], get_result_description(result));
        return PROCESS_FAILURE;
    }

    return PROCESS_SUCCESS;
}
