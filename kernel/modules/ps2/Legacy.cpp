#include "ps2/Legacy.h"

Iter legacy_scan(IterFunc<LegacyAddress> callback)
{
    if (callback(LEGACY_MOUSE) == Iter::STOP)
    {
        return Iter::STOP;
    }

    if (callback(LEGACY_KEYBOARD) == Iter::STOP)
    {
        return Iter::STOP;
    }

    if (callback(LEGACY_ATA0) == Iter::STOP)
    {
        return Iter::STOP;
    }

    if (callback(LEGACY_ATA1) == Iter::STOP)
    {
        return Iter::STOP;
    }

    if (callback(LEGACY_ATA2) == Iter::STOP)
    {
        return Iter::STOP;
    }

    if (callback(LEGACY_ATA3) == Iter::STOP)
    {
        return Iter::STOP;
    }

    if (callback(LEGACY_COM1) == Iter::STOP)
    {
        return Iter::STOP;
    }

    if (callback(LEGACY_COM2) == Iter::STOP)
    {
        return Iter::STOP;
    }

    if (callback(LEGACY_COM3) == Iter::STOP)
    {
        return Iter::STOP;
    }

    if (callback(LEGACY_COM4) == Iter::STOP)
    {
        return Iter::STOP;
    }

    return Iter::CONTINUE;
}
