#include <skift/__plugs__.h>
#include <skift/error.h>
#include <skift/process.h>
#include <skift/assert.h>

int process_this(void)
{
    return __plug_process_this();
}

int process_exec(const char *file_name, const char **argv)
{
    return __plug_process_exec(file_name, argv);
}

// TODO: void process_spawn();

int process_exit(int code)
{
    return __plug_process_exit(code);
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

int process_get_cwd(char* buffer, uint size)
{
    return __plug_process_get_cwd(buffer, size);
}

int process_set_cwd(const char* cwd)
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

int process_wait(int pid, int* exit_value)
{
    return __plug_process_wait(pid, exit_value);
}