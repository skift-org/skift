#include <stdio.h>
#include <skift/logger.h>
#include <skift/process.h>
#include <skift/thread.h>

int init_exec(const char* filename)
{
    sk_log(LOG_INFO, "Starting '%s'", filename);
    int process = sk_process_exec(filename, NULL);

    if (process == 0)
    {
        sk_log(LOG_WARNING, "Failed to start process: '%s'!", filename);
    }
    else
    {
        sk_log(LOG_FINE, "'%s' started!", filename);
    }

    return process;
}

int main(int argc, char **argv)
{
    (void)argc; (void)argv;
    printf("\033[0;36mWelcome to skiftOS\033[0m\n");

    int nyancat = init_exec("/bin/maker.hideo.compositor");
    sk_thread_wait(nyancat);

    return 0;
}
