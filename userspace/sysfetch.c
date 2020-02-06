/* Copyright © 2018-2020 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <libsystem/io/Stream.h>
#include <libsystem/system.h>

int main(int argc, char **argv)
{
    __unused(argc);
    __unused(argv);

    // FIXME: - get the uptime from the kernel.
    //        - get user and the machine name from the system

    SystemInfo info = system_get_info();
    SystemStatus status = system_get_status();

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
    printf("\e[16C UPTIME: 00:00\n");
    printf("\e[16C SHELL: /bin/Shell\n");
    printf("\e[16C TERMINAL: /bin/term\n");
    printf("\e[16C MEMORY: \e[m%dMib / %dMib\n", status.used_ram / (1024 * 1024), status.total_ram / (1024 * 1024));

    printf("\n");
    printf("\e[16C \e[40m  \e[41m  \e[42m  \e[43m  \e[44m  \e[45m  \e[46m  \e[47m  \e[m\n");
    printf("\n");

    return 0;
}
