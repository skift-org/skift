/* Copyright © 2018-2019 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <libsystem/__plugs__.h>
#include <libsystem/assert.h>
#include <libkernel/syscalls.h>

int __plug_process_this(void)
{
    return __syscall(SYS_PROCESS_SELF, 0, 0, 0, 0, 0);
}

int __plug_process_exec(const char *file_name, const char **argv)
{
    return __syscall(SYS_PROCESS_EXEC, (int)file_name, (int)argv, 0, 0, 0);
}

// TODO: void __plug_process_spawn();

int __plug_process_exit(int code)
{
    return __syscall(SYS_PROCESS_EXIT, code, 0, 0, 0, 0);
}

int __plug_process_cancel(int pid)
{
    return __syscall(SYS_PROCESS_CANCEL, pid, 0, 0, 0, 0);
}

int __plug_process_map(uint addr, uint count)
{
    return __syscall(SYS_PROCESS_MAP, addr, count, 0, 0, 0);
}

int __plug_process_unmap(uint addr, uint count)
{
    return __syscall(SYS_PROCESS_UNMAP, addr, count, 0, 0, 0);
}

uint __plug_process_alloc(uint count)
{
    return __syscall(SYS_PROCESS_ALLOC, count, 0, 0, 0, 0);
}

int __plug_process_free(uint addr, uint count)
{
    return __syscall(SYS_PROCESS_FREE, addr, count, 0, 0, 0);
}

int __plug_process_get_cwd(char *buffer, uint size)
{
    return __syscall(SYS_PROCESS_GET_CWD, (int)buffer, size, 0, 0, 0);
}

int __plug_process_set_cwd(const char *cwd)
{
    return __syscall(SYS_PROCESS_SET_CWD, (int)cwd, 0, 0, 0, 0);
}

int __plug_process_sleep(int time)
{
    return __syscall(SYS_PROCESS_SLEEP, time, 0, 0, 0, 0);
}

int __plug_process_wakeup(int pid)
{
    return __syscall(SYS_PROCESS_WAKEUP, pid, 0, 0, 0, 0);
}

int __plug_process_wait(int pid, int *exit_value)
{
    return __syscall(SYS_PROCESS_WAIT, pid, (int)exit_value, 0, 0, 0);
}