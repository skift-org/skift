/* Copyright © 2018-2019 N. Van Bossuyt.                                               */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <stdio.h>

int main(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    printf("\033[H\033[2J");

    return 0;
}
