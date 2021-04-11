#include <abi/Syscalls.h>

#include <libsystem/io/Stream.h>

#include <stdio.h>

int main(int argc, char const *argv[])
{
    UNUSED(argc);
    UNUSED(argv);

    SystemStatus status{};
    hj_system_status(&status);

    ElapsedTime seconds = status.uptime;

    printf("Up ");

    if (seconds / 86400 > 0)
    {
        printf("%d day%s, ", seconds / 86400, (seconds / 86400) == 1 ? "" : "s");
        seconds %= 86400;
    }

    if (seconds / 3600 > 0)
    {
        printf("%d hour%s, ", seconds / 3600, (seconds / 3600) == 1 ? "" : "s");
        seconds %= 3600;
    }

    if (seconds / 60 > 0)
    {
        printf("%d minute%s, ", seconds / 60, (seconds / 60) == 1 ? "" : "s");
        seconds %= 60;
    }

    printf("%d second%s\n", seconds, seconds == 1 ? "" : "s");

    return PROCESS_SUCCESS;
}
