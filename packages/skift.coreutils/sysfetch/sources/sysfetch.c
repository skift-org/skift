/* Copyright © 2018-2019 N. Van Bossuyt.                                               */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <stdio.h>

int main(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    printf("\033[1;34m    _____   \033[1;34muser\033[1;37m@\033[1;34mcore\n");
    printf("\033[1;34m   / ____|  \033[1;37mOS:       \033[0;37mskift\n");
    printf("\033[1;34m  | (___    \033[1;37mKERNEL:   \033[0;37mhjert\n");
    printf("\033[1;34m   \\___ \\   \033[1;37mUPTIME:   \033[0;37m00:00\n");
    printf("\033[1;34m   ____) |  \033[1;37mSHELL:    \033[0;37m/bin/sh\n");
    printf("\033[1;34m  |_____/   \n");
    printf("\033[1;34m            \033[1;30;40m##\033[1;41;31m##\033[1;42;32m##\033[1;43;33m##\033[1;44;34m##\033[1;45;35m##\033[1;46;36m##\033[1;47;37m##\033[0m\n");

    return 0;
}
