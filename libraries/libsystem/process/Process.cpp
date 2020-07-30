
#include <libsystem/Assert.h>
#include <libsystem/Result.h>
#include <libsystem/core/Plugs.h>
#include <libsystem/process/Launchpad.h>

int process_this()
{
    return __plug_process_this();
}

Result process_run(const char *command, int *pid)
{
    Launchpad *launchpad = launchpad_create("shell", "/Applications/shell/shell");

    launchpad_argument(launchpad, "-c");
    launchpad_argument(launchpad, command);

    return launchpad_launch(launchpad, pid);
}

void __no_return process_exit(int code)
{
    __plug_process_exit(code);

    ASSERT_NOT_REACHED();
}

Result process_cancel(int pid)
{
    return __plug_process_cancel(pid);
}

Result process_map(uintptr_t address, size_t size)
{
    return __plug_process_map(address, size);
}

Result process_alloc(size_t size, uintptr_t *out_address)
{
    return __plug_process_alloc(size, out_address);
}

Result process_free(uintptr_t address, size_t size)
{
    return __plug_process_free(address, size);
}

Result process_get_directory(char *buffer, size_t size)
{
    return __plug_process_get_directory(buffer, size);
}

Result process_set_directory(const char *directory)
{
    return __plug_process_set_directory(directory);
}

Result process_sleep(int time)
{
    return __plug_process_sleep(time);
}

int process_wait(int pid, int *exit_value)
{
    return __plug_process_wait(pid, exit_value);
}
