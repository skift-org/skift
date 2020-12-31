
#include <libsystem/io/Stream.h>

int main(int argc, char **argv)
{
    __unused(argc);
    __unused(argv);

    printf("\e[H\e[2J");

    return PROCESS_SUCCESS;
}
