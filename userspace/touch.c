/* Copyright © 2018-2019 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <libsystem/iostream.h>

int main(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    for (int i = 1; i < argc; i++)
    {
        IOStream *stream = iostream_open(argv[i], OPEN_CREATE);
        iostream_close(stream);
    }

    return 0;
}
