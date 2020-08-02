#include <abi/Syscalls.h>

#include <libsystem/core/CString.h>

int main(int argc, char **argv)
{
    if (argc == 2 && strcmp(argv[1], "--reboot") == 0)
        __syscall(SYS_SYSTEM_REBOOT, 0, 0, 0, 0, 0);

    return 0;
}
