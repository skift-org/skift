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

static inline RefPtr<RessourceMonitor> ressource_monitor(Callback<void(void)> on_click)
{
    auto ressource_monitor = make<RessourceMonitor>();

    if (on_click)
    {
        ressource_monitor->on(Widget::Event::ACTION, [on_click](auto) { on_click(); });
    }

    return ressource_monitor;
}

} // namespace panel