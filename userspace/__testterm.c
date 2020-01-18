/* Copyright © 2018-2019 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <libsystem/iostream.h>

int main(int argc, char **argv)
{
    __unused(argc);
    __unused(argv);

    printf("\e[m");

    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            printf("\e[%d;%dm##", 30 + i, 40 + j);
        }
        printf("\e[0m\n");
    }

    printf("\n");

    for (int i = 0; i < 8; i++)
    {
        printf("\e[1m");
        for (int j = 0; j < 8; j++)
        {
            printf("\e[%d;%dm##", 90 + i, 100 + j);
        }
        printf("\e[0m\n");
    }

    return 0;
}
