/* dstart.c: start a process as a daemon                                      */

#include <skift/process.h>
#include <skift/error.h>
#include <skift/iostream.h>

#include <hjert/task.h>

int main(int argc, char const *argv[])
{
    if (argc >= 2)
    {
        const char* deamon_argv[TASK_ARGV_COUNT];

        for (int i = 0; i < TASK_ARGV_COUNT - 1; i++)
        {
            deamon_argv[i] = argv[i+1];
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
    }
}
