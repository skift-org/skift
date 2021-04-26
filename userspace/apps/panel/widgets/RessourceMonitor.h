#pragma once

#include <libasync/Timer.h>

namespace panel
{

class RessourceMonitor : public Widget::ButtonElement
{
private:
    OwnPtr<Async::Timer> _ram_timer;
    OwnPtr<Async::Timer> _cpu_timer;

public:
    RessourceMonitor();
};

} // namespace panel