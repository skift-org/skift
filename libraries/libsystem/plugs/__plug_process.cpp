
#include <abi/Syscalls.h>

#include <libsystem/Assert.h>
#include <libsystem/core/Plugs.h>

static int _cached_this = -1;

int __plug_process_this()
{
    if (_cached_this == -1)
    {
        assert(__syscall(SYS_PROCESS_THIS, (int)&_cached_this, 0, 0, 0, 0) == SUCCESS);
    }

    return _cached_this;
}

Result __plug_process_launch(Launchpad *launchpad, int *pid)
{
    return (Result)__syscall(SYS_PROCESS_LAUNCH, (int)launchpad, (int)pid, 0, 0, 0);
}

void __plug_process_exit(int code)
{
    __syscall(SYS_PROCESS_EXIT, code, 0, 0, 0, 0);

    ASSERT_NOT_REACHED();
}

Result __plug_process_cancel(int pid)
{
    return (Result)__syscall(SYS_PROCESS_CANCEL, pid, 0, 0, 0, 0);
}

Result __plug_process_get_directory(char *buffer, uint size)
{
    return (Result)__syscall(SYS_PROCESS_GET_DIRECTORY, (int)buffer, size, 0, 0, 0);
}

Result __plug_process_set_directory(const char *directory)
{
    return (Result)__syscall(SYS_PROCESS_SET_DIRECTORY, (int)directory, 0, 0, 0, 0);
}

Result __plug_process_sleep(int time)
{
    return (Result)__syscall(SYS_PROCESS_SLEEP, time, 0, 0, 0, 0);
}

Result __plug_process_wait(int pid, int *exit_value)
{
    return (Result)__syscall(SYS_PROCESS_WAIT, pid, (int)exit_value, 0, 0, 0);
}
