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

    printf("\e[1;34m    _____   \e[1;34muser\e[1;37m@\e[1;34m%s\n", info.machine);
    printf("\e[1;34m   / ____|  \e[1;37mOS:       \e[0;37m%s\n", info.system_name);
    printf("\e[1;34m  | (___    \e[1;37mKERNEL:   \e[0;37m%s %s\n", info.kernel_name, info.kernel_release);
    printf("\e[1;34m   \\___ \\   \e[1;37mUPTIME:   \e[0;37m00:00\n");
    printf("\e[1;34m   ____) |  \e[1;37mSHELL:    \e[0;37m/bin/sh\n");
    printf("\e[1;34m  |_____/   \e[1;37mMEMORY:   \e[0;37m%dMib / %dMib\n", status.used_ram / (1024 * 1024), status.total_ram / (1024 * 1024));
    printf("\n");
    printf("\e[1;34m            \e[1;30;40m##\e[1;41;31m##\e[1;42;32m##\e[1;43;33m##\e[1;44;34m##\e[1;45;35m##\e[1;46;36m##\e[1;47;37m##\e[0m\n");

    return 0;
}
