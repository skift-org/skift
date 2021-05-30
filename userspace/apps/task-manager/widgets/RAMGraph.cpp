#include <libwidget/Elements.h>
#include <libwidget/Layouts.h>

#include "task-manager/widgets/RAMGraph.h"

using namespace Widget;
using namespace Graphic;

namespace TaskManager
{

RAMGraphComponent::RAMGraphComponent(RefPtr<TaskModel> tasks) : _tasks{tasks}
{
    _usage = make<Widget::GraphModel>();

    _update = own<Async::Timer>(500, [this] {
        SystemStatus status{};
        hj_system_status(&status);
        _usage->record(status.used_ram / (float)status.total_ram);
    });

    _update->start();
}

RefPtr<Element> RAMGraphComponent::build()
{
    // clang-format off

    return stack({
        fill({
            graph(_usage, Colors::ROYALBLUE),
            refresher(1000, [this] {
                SystemStatus status{};
                hj_system_status(&status);

                auto usage = status.used_ram / 1024 / 1024;
                auto available = status.total_ram / 1024 / 1024;

                return spacing(6,
                    vflow(4, {
                        hflow(4, {
                            icon(Graphic::Icon::get("chip")),
                            label("Memory")
                        }),
                        spacer(),
                        label(IO::format("Usage: {} Mio", usage),Math::Anchor::RIGHT),
                        label(IO::format("Available: {} Mio", available),Math::Anchor::RIGHT),
                        label(IO::format("Most greedy: {}", _tasks->ram_greedy()),Math::Anchor::RIGHT),
                    })
                );
            })
        }),
    });

    // clang-format on
}

}; // namespace TaskManager
