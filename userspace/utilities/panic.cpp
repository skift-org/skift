#include <libsystem/Common.h>

int main(int argc, char **argv)
{
    UNUSED(argc);
    UNUSED(argv);

    // do a page fault
    asm("int $1");

    return PROCESS_SUCCESS;
}
