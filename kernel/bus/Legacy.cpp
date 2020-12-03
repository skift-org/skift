#include "kernel/bus/Legacy.h"

Iteration legacy_scan(IterationCallback<LegacyAddress> callback)
{
    if (callback(LEGACY_MOUSE) == Iteration::STOP)
    {
        return Iteration::STOP;
    }

    if (callback(LEGACY_KEYBOARD) == Iteration::STOP)
    {
        return Iteration::STOP;
    }

    if (callback(LEGACY_COM1) == Iteration::STOP)
    {
        return Iteration::STOP;
    }

    if (callback(LEGACY_COM2) == Iteration::STOP)
    {
        return Iteration::STOP;
    }

    if (callback(LEGACY_COM3) == Iteration::STOP)
    {
        return Iteration::STOP;
    }

    if (callback(LEGACY_COM4) == Iteration::STOP)
    {
        return Iteration::STOP;
    }

    if (callback(LEGACY_SPEAKER) == Iteration::STOP)
    {
        return Iteration::STOP;
    }

    return Iteration::CONTINUE;
}
