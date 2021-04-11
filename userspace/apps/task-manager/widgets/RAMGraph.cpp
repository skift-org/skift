#include <abi/Syscalls.h>

#include <libio/Format.h>
#include <libsystem/system/System.h>
#include <libutils/StringBuilder.h>
#include <libwidget/Container.h>
#include <libwidget/IconPanel.h>

#include "task-manager/widgets/RAMGraph.h"

namespace task_manager
{

RAMGraph::RAMGraph(Component *parent, RefPtr<TaskModel> model)
    : Graph(parent, 256, Graphic::Colors::ROYALBLUE),
      _model(model)
{
    layout(VFLOW(0));
    insets(Insetsi(8));
    flags(Component::FILL);

    auto icon_and_text = new Widget::Container(this);
    icon_and_text->layout(HFLOW(4));
    new Widget::IconPanel(icon_and_text, Graphic::Icon::get("chip"));
    new Widget::Label(icon_and_text, "Memory");

    auto cpu_filler = new Widget::Container(this);
    cpu_filler->flags(Component::FILL);

    _label_usage = new Widget::Label(this, "Usage: nil Mio", Anchor::RIGHT);
    _label_available = new Widget::Label(this, "Available: nil Mio", Anchor::RIGHT);
    _label_greedy = new Widget::Label(this, "Most greedy: nil", Anchor::RIGHT);

    _graph_timer = own<Timer>(500, [&]() {
        SystemStatus status{};
        hj_system_status(&status);

        record(status.used_ram / (float)status.total_ram);
    });

    _graph_timer->start();

    _text_timer = own<Timer>(1000, [&]() {
        SystemStatus status{};
        hj_system_status(&status);

        unsigned usage = status.used_ram / 1024 / 1024;
        _label_usage->text(IO::format("Usage: {} Mio", usage));

        unsigned available = status.total_ram / 1024 / 1024;
        _label_available->text(IO::format("Available: {} Mio", available));

        auto greedy = _model->ram_greedy();
        _label_greedy->text(IO::format("Most greedy: {}", greedy));
    });

    _text_timer->start();
}

} // namespace task_manager