#include <stdio.h>
#include <string.h>

#include "kernel/protocol.h"

#include <skift/logger.h>
#include <skift/process.h>
#include <skift/thread.h>
#include <skift/messaging.h>

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
    printf("Welcome to \033[0;34mskift\033[0m!\n");

    int shell = init_exec("/bin/sh");
    sk_thread_wait_process(shell);

    return 0;
}
