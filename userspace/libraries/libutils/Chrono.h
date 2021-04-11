#pragma once

#include <abi/Syscalls.h>
#include <libsystem/Logger.h>

namespace Utils
{

class Chrono
{
private:
    const char *_name;
    Tick _start;

    NONMOVABLE(Chrono);
    NONCOPYABLE(Chrono);

public:
    Chrono(const char *name)
    {
        _name = name;
        hj_system_tick(&_start);
    }

    ~Chrono()
    {
        Tick end;
        hj_system_tick(&end);

        logger_trace("Chrono '%s' ended at %u", _name, end - _start);
    }
};

} // namespace Utils
