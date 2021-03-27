#include <abi/Syscalls.h>
#include <skift/Environment.h>

#include <libsystem/eventloop/Timer.h>
#include <libwidget/Graph.h>
#include <libwidget/Label.h>

#include "panel/widgets/RessourceMonitor.h"

namespace panel
{

RessourceMonitor::RessourceMonitor(Component *parent)
    : Button(parent, Button::TEXT)
{
    layout(VGRID(1));
    insets(0);

    auto ram_graph = new Widget::Graph(this, 50, Graphic::Colors::ROYALBLUE);
    new Widget::Label(ram_graph, "RAM", Anchor::CENTER);

    auto cpu_graph = new Widget::Graph(this, 50, Graphic::Colors::SEAGREEN);
    new Widget::Label(cpu_graph, "CPU", Anchor::CENTER);

    _ram_timer = own<Timer>(500, [ram_graph]() {
        SystemStatus status{};
        hj_system_status(&status);

        ram_graph->record(status.used_ram / (float)status.total_ram);
    });

    _cpu_timer = own<Timer>(100, [cpu_graph]() {
        SystemStatus status{};
        hj_system_status(&status);
        cpu_graph->record(status.cpu_usage / 100.0);
    });

    _ram_timer->start();
    _cpu_timer->start();
}

} // namespace panel