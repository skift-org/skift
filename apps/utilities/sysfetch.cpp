#include <abi/Syscalls.h>

#include <libsystem/io/Stream.h>
#include <stdio.h>

int main(int argc, char **argv)
{
    __unused(argc);
    __unused(argv);

    SystemInfo info{};
    hj_system_info(&info);

    printf("\e[1;94m");
    printf("       ___      \n");
    printf("      /\\  \\     \n");
    printf("     /::\\  \\    \n");
    printf("    /:/\\ \\  \\   \n");
    printf("   _\\:\\~\\ \\  \\  \n");
    printf("  /\\ \\:\\ \\ \\__\\ \n");
    printf("  \\:\\ \\:\\ \\/__/ \n");
    printf("   \\:\\ \\:\\__\\   \n");
    printf("    \\:\\/:/  /   \n");
    printf("     \\::/  /    \n");
    printf("      \\/__/     \n");
    printf("\e[m");

    printf("\e[11A");
    printf("\e[16C user@%s\n", info.machine);
    printf("\e[16C ------------\n");
    printf("\e[16C OS: %sOS\n", info.system_name);
    printf("\e[16C KERNEL: %s\n", info.kernel_name);
    printf("\e[16C VERSION: %s\n", info.kernel_release);
    printf("\e[16C BUILD: %s\n", info.kernel_build);
    printf("\e[16C UPTIME: ");

    SystemStatus status;
    hj_system_status(&status);

    ElapsedTime seconds = status.uptime;

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

    printf("\e[16C SHELL: /Applications/shell\n");
    printf("\e[16C TERMINAL: /Applications/terminal\n");
    printf("\e[16C COMPOSITOR: /Applications/compositor\n");
    printf("\e[16C MEMORY: \e[m%dMib / %dMib\n", (int)(status.used_ram / (1024 * 1024)), (int)(status.total_ram / (1024 * 1024)));

    printf("\n");
    printf("\e[16C \e[40m  \e[41m  \e[42m  \e[43m  \e[44m  \e[45m  \e[46m  \e[47m  \e[m\n");

    return PROCESS_SUCCESS;
}
