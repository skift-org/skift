#include <abi/Syscalls.h>

#include <libutils/StringBuilder.h>

#include <libsystem/eventloop/Timer.h>
#include <libsystem/system/System.h>

#include <libwidget/Container.h>
#include <libwidget/IconPanel.h>
#include <libwidget/Label.h>

#include "task-manager/widgets/CPUGraph.h"

namespace task_manager
{

CPUGraph::CPUGraph(Widget *parent, RefPtr<TaskModel> model)
    : Graph(parent, 256, Colors::SEAGREEN),
      _model(model)
{
    layout(VFLOW(0));
    insets(Insetsi(8));
    flags(Widget::FILL);

    auto icon_and_text = new Container(this);
    icon_and_text->layout(HFLOW(4));
    new IconPanel(icon_and_text, Icon::get("memory"));
    new Label(icon_and_text, "Processor");

    auto cpu_filler = new Container(this);
    cpu_filler->flags(Widget::FILL);

    _label_average = new Label(this, "Average: nil%", Anchor::RIGHT);
    _label_greedy = new Label(this, "Most greedy: nil", Anchor::RIGHT);
    _label_uptime = new Label(this, "Uptime: nil", Anchor::RIGHT);

    _graph_timer = own<Timer>(100, [&]() {
        SystemStatus status{};
        hj_system_status(&status);

        record(status.cpu_usage / 100.0);
    });

    _graph_timer->start();

    _text_timer = own<Timer>(1000, [&]() {
        SystemStatus status{};
        hj_system_status(&status);

        auto greedy = _model->cpu_greedy();

        char buffer_average[50];
        snprintf(buffer_average, 200, "Average: %i%%", (int)(average() * 100.0));
        _label_average->text(buffer_average);

        _label_greedy->text(StringBuilder().append("Most greedy: ").append(greedy).finalize());

        ElapsedTime seconds = status.uptime;
        int days = seconds / 86400;
        seconds %= 86400;
        int hours = seconds / 3600;
        seconds %= 3600;
        int minutes = seconds / 60;
        seconds %= 60;

        char buffer_uptime[50];
        snprintf(buffer_uptime, 50, "Uptime: %3d:%02d:%02d:%02d", days, hours, minutes, seconds);
        _label_uptime->text(buffer_uptime);
    });

    _text_timer->start();
}

} // namespace task_manager