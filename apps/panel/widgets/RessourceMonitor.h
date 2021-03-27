#pragma once

#include <libsystem/eventloop/Timer.h>

#include <libwidget/Button.h>

namespace panel
{

class RessourceMonitor : public Widget::Button
{
private:
    OwnPtr<Timer> _ram_timer;
    OwnPtr<Timer> _cpu_timer;

public:
    RessourceMonitor(Widget::Component *parent);
};

} // namespace panel