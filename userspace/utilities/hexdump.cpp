#include <libio/File.h>
#include <libio/Hexdump.h>
#include <libio/Streams.h>

int main(int argc, char **argv)
{
    if (argc == 1)
    {
        IO::errln("Usage: {} FILENAME", argv[0]);
        return PROCESS_FAILURE;
    }

    IO::File file{argv[1], OPEN_READ};

    if (file.result() != SUCCESS)
    {
        IO::errln("Couldn't read {}: {}", argv[1], get_result_description(file.result()));
        return PROCESS_FAILURE;
    }

    auto result = IO::hexdump(file, IO::out());

    return result.result() == SUCCESS ? PROCESS_SUCCESS : PROCESS_FAILURE;
}
