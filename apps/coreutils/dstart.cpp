
/* dstart.c: start a process as a daemon                                      */

#include <libsystem/io/Stream.h>
#include <libsystem/process/Launchpad.h>

int main(int argc, char const *argv[])
{
    if (argc == 1)
    {
        stream_format(err_stream, "dstart: No executable specified!\n");
        return PROCESS_FAILURE;
    }

    Launchpad *launchpad = launchpad_create(argv[1], argv[1]);

    for (int i = 1; i < argc; i++)
    {
        launchpad_argument(launchpad, argv[i]);
    }

    int pid = -1;
    Result result = launchpad_launch(launchpad, &pid);

    if (result < 0)
    {
        stream_format(err_stream, "dstart: Failed to start %s: %s\n", argv[1], get_result_description(result));
        return PROCESS_FAILURE;
    }

    return PROCESS_SUCCESS;
}
