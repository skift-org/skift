#include <libwidget/Layouts.h>
#include <libwidget/Views.h>

#include "panel/widgets/RessourceMonitor.h"

using namespace Widget;

namespace Panel
{

RessourceMonitor::RessourceMonitor()
{
    _ram_model = make<GraphModel>(50);
    _ram_timer = own<Async::Timer>(700, [this] {
        SystemStatus status{};
        hj_system_status(&status);
        _ram_model->record(status.used_ram / (float)status.total_ram);
    });

    _cpu_model = make<GraphModel>(50);
    _cpu_timer = own<Async::Timer>(300, [this] {
        SystemStatus status{};
        hj_system_status(&status);
        _cpu_model->record(status.cpu_usage / 100.0);
    });

    _ram_timer->start();
    _cpu_timer->start();
}

RefPtr<Element> RessourceMonitor::build()
{

    return hflow({
        stack({
            fill(graph(_cpu_model, Graphic::Colors::SEAGREEN)),
            fill(spacing({0, 12}, label("CPU", Math::Anchor::CENTER))),
        }),
        separator(),
        stack({
            fill(graph(_ram_model, Graphic::Colors::ROYALBLUE)),
            fill(spacing({0, 12}, label("RAM", Math::Anchor::CENTER))),
        }),
    });
}

} // namespace Panel
