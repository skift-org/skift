/* Copyright © 2018-2020 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <libsystem/io/Stream.h>

int main(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    printf("\e[H\e[2J");

    return 0;
}
