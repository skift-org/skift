/* Copyright © 2018-2019 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <skift/iostream.h>
#include <skift/system.h>

int main(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    // FIXME: - get the uptime from the kernel.
    //        - get user and the machine name from the system

    system_info_t info;
    system_status_t status;

    system_get_info(&info);
    system_get_status(&status);

    printf("\033[1;34m    _____   \033[1;34muser\033[1;37m@\033[1;34m%s\n", info.machine);
    printf("\033[1;34m   / ____|  \033[1;37mOS:       \033[0;37m%s\n", info.system_name);
    printf("\033[1;34m  | (___    \033[1;37mKERNEL:   \033[0;37m%s %s\n", info.kernel_name, info.kernel_release);
    printf("\033[1;34m   \\___ \\   \033[1;37mUPTIME:   \033[0;37m00:00\n");
    printf("\033[1;34m   ____) |  \033[1;37mSHELL:    \033[0;37m/bin/sh\n");
    printf("\033[1;34m  |_____/   \033[1;37mMEMORY:   \033[0;37m%dMib / %dMib\n", status.used_ram / (1024 * 1024), status.total_ram / (1024 * 1024));
    printf("\n");
    printf("\033[1;34m            \033[1;30;40m##\033[1;41;31m##\033[1;42;32m##\033[1;43;33m##\033[1;44;34m##\033[1;45;35m##\033[1;46;36m##\033[1;47;37m##\033[0m\n");

    return 0;
}
