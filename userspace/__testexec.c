/* Copyright © 2018-2019 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <libsystem/iostream.h>
#include <libsystem/process.h>

#define LOOP_MAX 1000

int main(int argc, char **argv)
{
    __unused(argc);
    __unused(argv);

    printf("\n");

    for (int i = 0; i < LOOP_MAX; i++)
    {
        printf("\r%d out %d... ", i + 1, LOOP_MAX);
        int pid = process_exec("/bin/__testapp", (const char *[]){"__testapp", NULL});
        process_wait(pid, NULL);
    }

    printf("PASS\n");

    return 0;
}
