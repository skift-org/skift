#include <libio/Streams.h>
#include <libutils/Path.h>

int main(int argc, char **argv)
{
    if (argc == 1)
    {
        IO::errln("usage: %s path", argv[0]);
        return PROCESS_FAILURE;
    }

    IO::outln("{}", Path::parse(argv[1]).basename());

    return PROCESS_SUCCESS;
}
