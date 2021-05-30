#include <libwidget/Elements.h>
#include <libwidget/Layouts.h>

#include "task-manager/widgets/CPUGraph.h"

using namespace Widget;
using namespace Graphic;

namespace TaskManager
{

CPUGraphComponent::CPUGraphComponent(RefPtr<TaskModel> tasks) : _tasks{tasks}
{
    _usage = make<Widget::GraphModel>();

    _update = own<Async::Timer>(500, [this] {
        SystemStatus status{};
        hj_system_status(&status);
        _usage->record(status.cpu_usage / 100.0);
    });

    _update->start();
}

RefPtr<Element> CPUGraphComponent::build()
{
    // clang-format off

    return stack({
        fill({
            graph(_usage, Colors::SEAGREEN),
            refresher(1000, [this] {
                SystemStatus status{};
                hj_system_status(&status);

                ElapsedTime seconds = status.uptime;
                int days = seconds / 86400;
                seconds %= 86400;
                int hours = seconds / 3600;
                seconds %= 3600;
                int minutes = seconds / 60;
                seconds %= 60;

                return spacing(6,
                    vflow(4, {
                        hflow(4, {
                            icon(Graphic::Icon::get("memory")),
                            label("Processor")
                        }),
                        spacer(),
                        label(IO::format("Average: {}%", (int)(_usage->average() * 100)),Math::Anchor::RIGHT),
                        label(IO::format("Most greedy: {}", _tasks->cpu_greedy()),Math::Anchor::RIGHT),
                        label(IO::format("Uptime: {03}:{02}:{02}:{02}", days, hours, minutes, seconds),Math::Anchor::RIGHT)
                    })
                );
            })
        }),
    });

    // clang-format on
}

}; // namespace TaskManager