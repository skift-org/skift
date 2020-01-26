#include <libsystem/__plugs__.h>
#include <libsystem/assert.h>
#include <libsystem/cstring.h>
#include <libsystem/process/Launchpad.h>

Launchpad *launchpad_create(const char *name, const char *executable)
{
    Launchpad *launchpad = __create(Launchpad);

    strcpy(launchpad->name, name);
    strcpy(launchpad->executable, executable);

    return launchpad;
}

void launchpad_destroy(Launchpad *launchpad)
{
    for (int i = 0; i < launchpad->argc; i++)
    {
        free(launchpad->argv[i]);
    }

    free(launchpad);
}

void launchpad_argument(Launchpad *launchpad, const char *argument)
{
    assert(launchpad->argc < PROCESS_ARG_COUNT);

    launchpad->argv[launchpad->argc] = strdup(argument);
    launchpad->argv[launchpad->argc + 1] = NULL;

    launchpad->argc++;
}

int launchpad_launch(Launchpad *launchpad)
{
    int process_handle = __plug_process_launch(launchpad);

    launchpad_destroy(launchpad);

    return process_handle;
}
