
#include <libsystem/io_new/Streams.h>

int main(int, const char *[])
{
    if (System::out("\e[H\e[2J").success())
    {
        return PROCESS_SUCCESS;
    }
    else
    {
        return PROCESS_FAILURE;
    }
}
