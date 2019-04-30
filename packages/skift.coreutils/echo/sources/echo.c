/* Copyright © 2018-2019 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <skift/iostream.h>

int main(int argc, char **argv)
{
    for(int i = 1; i < argc; i++)
    {
        puts(argv[i]);
        puts(" ");
    }

    return 0;
}
