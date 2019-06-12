/* Copyright © 2018-2019 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <skift/iostream.h>
#include <skift/process.h>

#define LOOP_MAX 1000

int main(int argc, char **argv)
{
    UNUSED(argc); UNUSED(argv);

    for (int i = 0; i < LOOP_MAX; i++)
    {
        printf("\n%d out %d...", i, LOOP_MAX);
        int pid = process_exec("/bin/sysfetch", (const char*[]){"sysfetch", NULL});
        process_wait(pid, NULL);
    }
    
    printf("\nPASS");

    return 0;
}
