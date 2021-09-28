#include "unix/UNIX.h"

Iter unix_scan(IterFunc<UNIXAddress> callback)
{
    if (callback(UNIX_ZERO) == Iter::STOP)
    {
        return Iter::STOP;
    }

    if (callback(UNIX_NULL) == Iter::STOP)
    {
        return Iter::STOP;
    }

    if (callback(UNIX_RANDOM) == Iter::STOP)
    {
        return Iter::STOP;
    }

    return Iter::CONTINUE;
}
