#include <abi/Syscalls.h>
#include <skift/Environment.h>

#include <libasync/Timer.h>
#include <libwidget/Graph.h>
#include <libwidget/Label.h>

#include "panel/widgets/RessourceMonitor.h"

namespace panel
{

RessourceMonitor::RessourceMonitor()
    : Button(Button::TEXT)
{
    layout(VGRID(1));
    insets(0);

    auto ram_graph = add<Widget::Graph>(50, Graphic::Colors::ROYALBLUE);
    ram_graph->add<Widget::Label>("RAM", Anchor::CENTER);

    auto cpu_graph = add<Widget::Graph>(50, Graphic::Colors::SEAGREEN);
    cpu_graph->add<Widget::Label>("CPU", Anchor::CENTER);

    _ram_timer = own<Async::Timer>(700, [ram_graph]() {
        SystemStatus status{};
        hj_system_status(&status);
        ram_graph->record(status.used_ram / (float)status.total_ram);
    });

    _cpu_timer = own<Async::Timer>(300, [cpu_graph]() {
        SystemStatus status{};
        hj_system_status(&status);
        cpu_graph->record(status.cpu_usage / 100.0);
    });

    _ram_timer->start();
    _cpu_timer->start();
}

} // namespace panel