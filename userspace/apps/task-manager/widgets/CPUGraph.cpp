#include <abi/Syscalls.h>

#include <libutils/StringBuilder.h>

#include <libasync/Timer.h>
#include <libio/Format.h>
#include <libsystem/system/System.h>

#include <libwidget/Container.h>
#include <libwidget/IconPanel.h>
#include <libwidget/Label.h>

#include "task-manager/widgets/CPUGraph.h"

namespace task_manager
{

CPUGraph::CPUGraph(Component *parent, RefPtr<TaskModel> model)
    : Graph(parent, 256, Graphic::Colors::SEAGREEN),
      _model(model)
{
    layout(VFLOW(0));
    insets(Insetsi(8));
    flags(Component::FILL);

    auto icon_and_text = new Widget::Container(this);
    icon_and_text->layout(HFLOW(4));
    new Widget::IconPanel(icon_and_text, Graphic::Icon::get("memory"));
    new Widget::Label(icon_and_text, "Processor");

    auto cpu_filler = new Widget::Container(this);
    cpu_filler->flags(Component::FILL);

    _label_average = new Widget::Label(this, "Average: nil%", Anchor::RIGHT);
    _label_greedy = new Widget::Label(this, "Most greedy: nil", Anchor::RIGHT);
    _label_uptime = new Widget::Label(this, "Uptime: nil", Anchor::RIGHT);

    _graph_timer = own<Async::Timer>(100, [&]() {
        SystemStatus status{};
        hj_system_status(&status);

        record(status.cpu_usage / 100.0);
    });

    _graph_timer->start();

    _text_timer = own<Async::Timer>(1000, [&]() {
        SystemStatus status{};
        hj_system_status(&status);

        auto greedy = _model->cpu_greedy();

        _label_average->text(IO::format("Average: {}%", (int)(average() * 100.0)));
        _label_greedy->text(IO::format("Most greedy: {}", greedy));

        ElapsedTime seconds = status.uptime;
        int days = seconds / 86400;
        seconds %= 86400;
        int hours = seconds / 3600;
        seconds %= 3600;
        int minutes = seconds / 60;
        seconds %= 60;

        _label_uptime->text(IO::format("Uptime: {03}:{02}:{02}:{02}", days, hours, minutes, seconds));
    });

    _text_timer->start();
}

} // namespace task_manager