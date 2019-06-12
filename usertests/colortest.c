/* Copyright © 2018-2019 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <skift/iostream.h>

int main(int argc, char **argv)
{
    UNUSED(argc); UNUSED(argv);

    printf("\033[m");

    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            printf("\033[%d;%dm##", 30 + i, 40 + j);
        }
        printf("\033[0m\n");
    }

    printf("\n");


    for (int i = 0; i < 8; i++)
    {
        printf("\033[1m");
        for (int j = 0; j < 8; j++)
        {
            printf("\033[%d;%dm##", 30 + i, 40 + j);
        }
        printf("\033[0m\n");
    }

    return 0;
}
