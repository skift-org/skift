/* Copyright © 2018-2020 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

/* dstart.c: start a process as a daemon                                      */

#include <libsystem/io/Stream.h>
#include <libsystem/process/Launchpad.h>

int main(int argc, char const *argv[])
{
    if (argc < 2)
    {
        stream_printf(err_stream, "dstart: No executable specified!\n");
        return -1;
    }

    Launchpad *launchpad = launchpad_create(argv[1], argv[1]);

    for (int i = 0; i < argc - 1; i++)
    {
        launchpad_argument(launchpad, argv[i + 1]);
    }

    int result = launchpad_launch(launchpad);

    if (result < 0)
    {
        stream_printf(err_stream, "dstart: Failled to start %s: %s\n", argv[1], result_to_string(-result));
        return -1;
    }

    return 0;
}
