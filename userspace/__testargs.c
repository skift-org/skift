/* Copyright © 2018-2019 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <libsystem/iostream.h>

int main(int argc, char **argv)
{
    printf("argc=%d\n", argc);
    for (int i=0; i < argc; i++)
    {
        printf("argv[%d]='%s'\n", i+1, argv[i]);
    }

    return 0;
}
