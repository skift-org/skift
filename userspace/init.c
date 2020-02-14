/* Copyright © 2018-2020 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <libsystem/filesystem.h>
#include <libsystem/io/Stream.h>
#include <libsystem/logger.h>
#include <libsystem/process/Launchpad.h>
#include <libsystem/process/Process.h>

int init_exec(const char *filename)
{
    logger_info("Starting '%s'...", filename);

    Launchpad *launchpad = launchpad_create(filename, filename);

    int process = launchpad_launch(launchpad);

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
    __unused(argc);
    __unused(argv);

    logger_level(LOGGER_TRACE);

    int term = init_exec("/bin/Compositor");

    process_wait(term, NULL);

    printf("\n\n\t\e[1;34mGoodbye!\e[m - n°1\n\n");

    return 0;
}
