
#include <libio/Streams.h>

int main(int argc, const char *argv[])
{
    for (int i = 1; i < argc; i++)
    {
        IO::out("{} ", argv[i]);
    }

    IO::out("\n");

    return PROCESS_SUCCESS;
}
