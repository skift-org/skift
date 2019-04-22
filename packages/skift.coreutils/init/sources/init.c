/* Copyright © 2018-2019 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <string.h>

#include "kernel/protocol.h"

#include <skift/logger.h>
#include <skift/process.h>
#include <skift/thread.h>
#include <skift/messaging.h>

int init_exec(const char* filename)
{
    sk_log(LOG_INFO, "Starting '%s'", filename);
    int process = sk_process_exec(filename, (const char*[]){filename, NULL});

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

    sk_logger_setlevel(LOG_ALL);

    int shell = init_exec("/bin/sh");
    
    sk_thread_wait_process(shell, NULL);

    return 0;
}
