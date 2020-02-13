/* Copyright © 2018-2020 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <libsystem/__plugs__.h>
#include <libsystem/assert.h>
#include <libsystem/Result.h>
#include <libsystem/process/Launchpad.h>

int process_this(void)
{
    return __plug_process_this();
}

int process_run(const char *command)
{
    Launchpad *launchpad = launchpad_create("/bin/Shell", "/bin/Shell");

    launchpad_argument(launchpad, "-c");
    launchpad_argument(launchpad, command);

    return launchpad_launch(launchpad);
}

void __attribute__((noreturn)) process_exit(int code)
{
    __plug_process_exit(code);

    ASSERT_NOT_REACHED();
}

int process_cancel(int pid)
{
    return __plug_process_cancel(pid);
}

int process_map(uintptr_t addr, size_t count)
{
    return __plug_process_map(addr, count);
}

int process_unmap(uintptr_t addr, size_t count)
{
    return __plug_process_map(addr, count);
}

uint process_alloc(size_t count)
{
    return __plug_process_alloc(count);
}

int process_free(uintptr_t addr, size_t count)
{
    return __plug_process_free(addr, count);
}

int process_get_cwd(char *buffer, size_t size)
{
    return __plug_process_get_cwd(buffer, size);
}

int process_set_cwd(const char *cwd)
{
    int result = __plug_process_set_cwd(cwd);
    RETURN_AND_SET_ERROR(result, result, -1);
}

int process_sleep(int time)
{
    return __plug_process_sleep(time);
}

int process_wakeup(int pid)
{
    return __plug_process_wakeup(pid);
}

int process_wait(int pid, int *exit_value)
{
    return __plug_process_wait(pid, exit_value);
}