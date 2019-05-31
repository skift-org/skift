/* dstart.c: start a process as a daemon                                      */

#include <skift/process.h>
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

        process_exec(argv[1], deamon_argv);
    }

    return 0;
}
