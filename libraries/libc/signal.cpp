#include <abi/Syscalls.h>
#include <assert.h>
#include <signal.h>

sighandler_t signal(int sig, sighandler_t handler)
{
    //TODO: connect this handler
    __unused(sig);
    __unused(handler);

    ASSERT_NOT_REACHED();

    return NULL;
}

int raise(int sig)
{
    switch (sig)
    {
    case SIGTERM:
    case SIGKILL:
        return hj_process_exit(-1) == Result::SUCCESS ? 0 : -1;
    default:
        return -1;
    }
}