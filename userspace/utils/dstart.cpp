#include <libio/Streams.h>
#include <libsystem/process/Launchpad.h>

/* dstart.c: start a process as a daemon                                      */

int main(int argc, char const *argv[])
{
    if (argc == 1)
    {
        IO::errln("dstart: No executable specified!");
        return PROCESS_FAILURE;
    }

    Launchpad *launchpad = launchpad_create(argv[1], argv[1]);

    for (int i = 1; i < argc; i++)
    {
        launchpad_argument(launchpad, argv[i]);
    }

    int pid = -1;
    HjResult result = launchpad_launch(launchpad, &pid);

    if (result < 0)
    {
        IO::errln("dstart: Failed to start {}: {}", argv[1], get_result_description(result));
        return PROCESS_FAILURE;
    }

    return PROCESS_SUCCESS;
}
