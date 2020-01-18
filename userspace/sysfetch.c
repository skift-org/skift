/* Copyright © 2018-2019 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <libsystem/iostream.h>
#include <libsystem/system.h>

int main(int argc, char **argv)
{
    __unused(argc);
    __unused(argv);

    // FIXME: - get the uptime from the kernel.
    //        - get user and the machine name from the system

    SystemInfo info = system_get_info();
    SystemStatus status = system_get_status();

    printf("\e[94m    _____   \e[97muser\e[37m@\e[94m%s\n", info.machine);
    printf("\e[94m   / ____|  \e[97mOS:       \e[m%s\n", info.system_name);
    printf("\e[94m  | (___    \e[97mKERNEL:   \e[m%s %s\n", info.kernel_name, info.kernel_release);
    printf("\e[94m   \\___ \\   \e[97mUPTIME:   \e[m00:00\n");
    printf("\e[94m   ____) |  \e[97mSHELL:    \e[m/bin/sh\n");
    printf("\e[94m  |_____/   \e[97mMEMORY:   \e[m%dMib / %dMib\n", status.used_ram / (1024 * 1024), status.total_ram / (1024 * 1024));
    printf("\n");
    printf("            \e[100m  \e[101m  \e[102m  \e[103m  \e[104m  \e[105m  \e[106m  \e[107m  \e[m\n");

    return 0;
}
