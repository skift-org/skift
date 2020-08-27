
#include <abi/Syscalls.h>

#include <libsystem/Assert.h>
#include <libsystem/core/Plugs.h>

static int _cached_this = -1;

int __plug_process_this()
{
    if (_cached_this == -1)
    {
        assert(__syscall(SYS_PROCESS_THIS, (int)&_cached_this) == SUCCESS);
    }

    return _cached_this;
}

Result __plug_process_launch(Launchpad *launchpad, int *pid)
{
    return __syscall(SYS_PROCESS_LAUNCH, (int)launchpad, (int)pid);
}

void __plug_process_exit(int code)
{
    __syscall(SYS_PROCESS_EXIT, code);

    ASSERT_NOT_REACHED();
}

Result __plug_process_cancel(int pid)
{
    return __syscall(SYS_PROCESS_CANCEL, pid);
}

Result __plug_process_get_directory(char *buffer, uint size)
{
    return __syscall(SYS_PROCESS_GET_DIRECTORY, (int)buffer, size);
}

Result __plug_process_set_directory(const char *directory)
{
    return __syscall(SYS_PROCESS_SET_DIRECTORY, (int)directory);
}

Result __plug_process_sleep(int time)
{
    return __syscall(SYS_PROCESS_SLEEP, time);
}

Result __plug_process_wait(int pid, int *exit_value)
{
    return __syscall(SYS_PROCESS_WAIT, pid, (int)exit_value);
}
