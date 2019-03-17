/* Copyright © 2018-2019 N. Van Bossuyt.                                               */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include "kernel/shared/syscalls.h"

int main(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    __syscall(SYS_PROCESS_EXIT, 0, 0, 0, 0, 0);

    return 0;
}
