/* Copyright © 2018-2019 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

/* dstart.c: start a process as a daemon                                      */

#include <abi/Process.h>

#include <libsystem/error.h>
#include <libsystem/io/Stream.h>
#include <libsystem/process.h>

int main(int argc, char const *argv[])
{
    if (argc >= 2)
    {
        const char *deamon_argv[PROCESS_ARG_COUNT];

        for (int i = 0; i < PROCESS_ARG_COUNT - 1; i++)
        {
            deamon_argv[i] = argv[i + 1];
        }

        if (process_exec(argv[1], deamon_argv) < 0)
        {
            error_print("Failled to start deamon");
            return -1;
        }
        else
        {
            return 0;
        }
    }
    else
    {
        printf("No executable specified!\n");
        return -1;
    }
}
