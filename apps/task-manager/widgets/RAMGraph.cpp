#include <abi/Syscalls.h>

#include <libutils/StringBuilder.h>

#include <libsystem/system/System.h>

#include <libwidget/Container.h>
#include <libwidget/IconPanel.h>

#include <stdio.h>

#include "task-manager/widgets/RAMGraph.h"

namespace task_manager
{

RAMGraph::RAMGraph(Widget *parent, RefPtr<TaskModel> model)
    : Graph(parent, 256, Colors::ROYALBLUE),
      _model(model)
{
    layout(VFLOW(0));
    insets(Insetsi(8));
    flags(Widget::FILL);

    auto icon_and_text = new Container(this);
    icon_and_text->layout(HFLOW(4));
    new IconPanel(icon_and_text, Icon::get("chip"));
    new Label(icon_and_text, "Memory");

    auto cpu_filler = new Container(this);
    cpu_filler->flags(Widget::FILL);

    _label_usage = new Label(this, "Usage: nil Mio", Anchor::RIGHT);
    _label_available = new Label(this, "Available: nil Mio", Anchor::RIGHT);
    _label_greedy = new Label(this, "Most greedy: nil", Anchor::RIGHT);

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
        _label_usage->text(String::format("Usage: {} Mio", usage));

        unsigned available = status.total_ram / 1024 / 1024;
        _label_available->text(String::format("Available: {} Mio", available));

        auto greedy = _model->ram_greedy();
        _label_greedy->text(String::format("Most greedy: {}", greedy));
    });

    _text_timer->start();
}

} // namespace task_manager