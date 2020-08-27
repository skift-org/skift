#include <abi/Syscalls.h>

#include <libutils/String.h>

int main(int argc, char **argv)
{
    if (argc == 2 && String(argv[1]) == "--reboot")
        __syscall(SYS_SYSTEM_REBOOT);

    return 0;
}
