#include <libsystem/Common.h>

int main(int argc, char **argv)
{
    __unused(argc);
    __unused(argv);

    // do a page fault
    asm("int $1");

    return PROCESS_SUCCESS;
}
