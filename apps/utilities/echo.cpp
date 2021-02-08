
#include <libsystem/io_new/Streams.h>

int main(int argc, const char *argv[])
{
    for (int i = 1; i < argc; i++)
    {
        System::out("{} ", argv[i]);
    }

    System::out("\n");

    return PROCESS_SUCCESS;
}
