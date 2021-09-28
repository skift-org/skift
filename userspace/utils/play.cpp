#include <libio/Copy.h>
#include <libio/File.h>
#include <libio/Streams.h>

#include "ac97/AC97.h"

int main(int argc, char **argv)
{
    if (argc == 1)
    {
        IO::errln("{}: Missing Audio file operand", argv[0]);
        return PROCESS_FAILURE;
    }

    IO::File file{argv[1], HJ_OPEN_READ};

    if (!file.exist())
    {
        return PROCESS_FAILURE;
    }

    IO::File device{"/Devices/sound", HJ_OPEN_WRITE | HJ_OPEN_CREATE};

    if (!device.exist())
    {
        return PROCESS_FAILURE;
    }

    if (IO::copy(file, device) != SUCCESS)
    {
        return PROCESS_FAILURE;
    }

    IO::outln("Finish Playing");

    return PROCESS_SUCCESS;
}
