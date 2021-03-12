
#include <libio/Streams.h>

int main(int, const char *[])
{
    if (IO::out("\e[H\e[2J").success())
    {
        return PROCESS_SUCCESS;
    }
    else
    {
        return PROCESS_FAILURE;
    }
}
