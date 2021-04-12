#pragma once

#include <libasync/Timer.h>

#include <libwidget/Button.h>

namespace panel
{

class RessourceMonitor : public Widget::Button
{
private:
    OwnPtr<Async::Timer> _ram_timer;
    OwnPtr<Async::Timer> _cpu_timer;

public:
    RessourceMonitor(Widget::Component *parent);
};

} // namespace panel