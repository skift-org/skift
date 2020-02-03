/* Copyright © 2018-2020 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <libsystem/assert.h>
#include <libsystem/io/Stream.h>
#include <libsystem/process/Process.h>

#define LOOP_MAX 1000

int main(int argc, char **argv)
{
    __unused(argc);
    __unused(argv);

    printf("\n");

    for (int i = 0; i < LOOP_MAX; i++)
    {
        printf("\r%d out %d... ", i + 1, LOOP_MAX);

        int process_id = process_run("sysfetch");

        assert(process_id > 0);

        process_wait(process_id, NULL);
    }

    printf("PASS\n");

    return 0;
}
