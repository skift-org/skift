#include <abi/Syscalls.h>
#include <libsystem/Logger.h>
#include <signal.h>

__sighandler signal(int sig, __sighandler handler)
{
    //TODO: connect this handler
    __unused(sig);
    __unused(handler);
    logger_error("Not connecting signal handler");

    return NULL;
}

int raise(int sig)
{
    logger_warn("Raise: %i", sig);

    switch (sig)
    {
    case SIGTERM:
    case SIGKILL:
        return hj_process_exit(-1) == Result::SUCCESS ? 0 : -1;
    default:
        logger_error("Unhandled signal: %i", sig);
        return -1;
    }
}