/* Copyright © 2018-2019 MAKER.                                               */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <stdio.h>

int main(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    printf("\033[m");

    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            printf("\033[%d;%dm##", 30 + i, 40 + j);
        }
        printf("\n");
    }

    printf("\n");

    printf("\033[1m");

    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            printf("\033[%d;%dm##", 30 + i, 40 + j);
        }
        printf("\n");
    }

    return 0;
}
