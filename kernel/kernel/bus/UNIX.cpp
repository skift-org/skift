#include "kernel/bus/UNIX.h"

Iteration unix_scan(IterationCallback<UNIXAddress> callback)
{
    if (callback(UNIX_ZERO) == Iteration::STOP)
    {
        return Iteration::STOP;
    }

    if (callback(UNIX_NULL) == Iteration::STOP)
    {
        return Iteration::STOP;
    }

    if (callback(UNIX_RANDOM) == Iteration::STOP)
    {
        return Iteration::STOP;
    }

    return Iteration::CONTINUE;
}
