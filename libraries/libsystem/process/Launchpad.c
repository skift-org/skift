/* Copyright © 2018-2020 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <libsystem/__plugs__.h>
#include <libsystem/assert.h>
#include <libsystem/cstring.h>
#include <libsystem/process/Launchpad.h>

Launchpad *launchpad_create(const char *name, const char *executable)
{
    Launchpad *launchpad = __create(Launchpad);

    strcpy(launchpad->name, name);
    strcpy(launchpad->executable, executable);

    for (size_t i = 0; i < PROCESS_HANDLE_COUNT; i++)
    {
        launchpad->handles[i] = HANDLE_INVALID_ID;
    }

    launchpad->handles[0] = 0;
    launchpad->handles[1] = 1;
    launchpad->handles[2] = 2;
    launchpad->handles[3] = 3;

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

void launchpad_handle(Launchpad *launchpad, Handle *handle_to_pass, int destination)
{
    assert(destination >= 0 && destination < PROCESS_ARG_COUNT);

    launchpad->handles[destination] = handle_to_pass->id;
}

int launchpad_launch(Launchpad *launchpad)
{
    int process_handle = __plug_process_launch(launchpad);

    launchpad_destroy(launchpad);

    return process_handle;
}
