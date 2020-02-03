/* Copyright © 2018-2020 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <libsystem/io/Stream.h>

int main(int argc, char **argv)
{
    for (int i = 1; i < argc; i++)
        printf("%s ", argv);

    return 0;
}
