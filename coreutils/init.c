
#include <libsystem/Filesystem.h>
#include <libsystem/Logger.h>
#include <libsystem/io/Stream.h>
#include <libsystem/process/Launchpad.h>
#include <libsystem/process/Process.h>

int init_exec(const char *filename)
{
    logger_info("Starting '%s'...", filename);

    Launchpad *launchpad = launchpad_create(filename, filename);

    int pid = -1;
    Result result = launchpad_launch(launchpad, &pid);

    if (result != SUCCESS)
    {
        logger_warn("Failed to start process: '%s'!", filename);
    }
    else
    {
        logger_info("'%s' started with pid=%d !", filename, pid);
    }

    return pid;
}

int main(int argc, char **argv)
{
    __unused(argc);
    __unused(argv);

    logger_level(LOGGER_TRACE);

    int term = init_exec("/Applications/compositor");

    process_wait(term, NULL);

    printf("\n\n\t\e[1;34mGoodbye!\e[m - n°1\n\n");

    return 0;
}
