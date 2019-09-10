/* Copyright © 2018-2019 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <libsystem/__plugs__.h>
#include <libsystem/error.h>
#include <libsystem/process.h>
#include <libsystem/assert.h>

int process_this(void)
{
    return __plug_process_this();
}

int process_exec(const char *file_name, const char **argv)
{
    int result = __plug_process_exec(file_name, argv);
    RETURN_AND_SET_ERROR(result, result, -1);
}

// TODO: void process_spawn();

void __attribute__((noreturn)) process_exit(int code)
{
    __plug_process_exit(code);
}

int process_cancel(int pid)
{
    return __plug_process_cancel(pid);
}

int process_map(uint addr, uint count)
{
    return __plug_process_map(addr, count);
}

int process_unmap(uint addr, uint count)
{
    return __plug_process_map(addr, count);
}

uint process_alloc(uint count)
{
    return __plug_process_alloc(count);
}

int process_free(uint addr, uint count)
{
    return __plug_process_free(addr, count);
}

int process_get_cwd(char *buffer, uint size)
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