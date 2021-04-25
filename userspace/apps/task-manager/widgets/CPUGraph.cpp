#include <abi/Syscalls.h>

#include <libutils/StringBuilder.h>

#include <libasync/Timer.h>
#include <libio/Format.h>
#include <libsystem/system/System.h>

#include <libwidget/Container.h>
#include <libwidget/Elements.h>
#include <libwidget/Label.h>

#include "task-manager/widgets/CPUGraph.h"

namespace task_manager
{

CPUGraph::CPUGraph(RefPtr<TaskModel> model)
    : Graph(256, Graphic::Colors::SEAGREEN),
      _model(model)
{
    layout(VFLOW(0));
    insets(Insetsi(8));
    flags(Element::FILL);

    auto icon_and_text = add<Widget::Container>();
    icon_and_text->layout(HFLOW(4));
    icon_and_text->add(Widget::icon("memory"));
    icon_and_text->add<Widget::Label>("Processor");

    auto cpu_filler = add<Widget::Container>();
    cpu_filler->flags(Element::FILL);

    _label_average = add<Widget::Label>("Average: nil%", Anchor::RIGHT);
    _label_greedy = add<Widget::Label>("Most greedy: nil", Anchor::RIGHT);
    _label_uptime = add<Widget::Label>("Uptime: nil", Anchor::RIGHT);

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