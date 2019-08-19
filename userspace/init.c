/* Copyright © 2018-2019 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <libsystem/cstring.h>
#include <libsystem/iostream.h>

#include <libsystem/logger.h>
#include <libsystem/process.h>
#include <libsystem/messaging.h>
#include <libsystem/filesystem.h>

int init_exec(const char *filename)
{
    logger_info("Starting '%s'", filename);
    int process = process_exec(filename, (const char *[]){filename, NULL});

    if (process < 0)
    {
        logger_warn("Failed to start process: '%s'!", filename);
    }
    else
    {
        logger_info("'%s' started with pid=%d !", filename, process);
    }

    return process;
}

int main(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    logger_level(LOGGER_TRACE);

    printf("Welcome to \033[1;34mskiftOS\033[0m!\n");
    iostream_flush(out_stream);

    filesystem_mkfifo("/dev/term");
    init_exec("/bin/term");
    int shell = init_exec("/bin/sh");

    process_wait(shell, NULL);

    printf("\n\e[1;34mGoodbye!");

    return 0;
}
