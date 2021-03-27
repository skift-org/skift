#pragma once

#include <libsystem/eventloop/Timer.h>

#include <libwidget/Button.h>

namespace panel
{

class RessourceMonitor : public Button
{
private:
    OwnPtr<Timer> _ram_timer;
    OwnPtr<Timer> _cpu_timer;

public:
    RessourceMonitor(Component *parent);
};

} // namespace panel