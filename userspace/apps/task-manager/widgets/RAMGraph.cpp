#include <abi/Syscalls.h>

#include <libio/Format.h>
#include <libsystem/system/System.h>
#include <libutils/StringBuilder.h>
#include <libwidget/Elements.h>

#include "task-manager/widgets/RAMGraph.h"

namespace task_manager
{

RAMGraph::RAMGraph(RefPtr<TaskModel> model)
    : Graph(256, Graphic::Colors::ROYALBLUE),
      _model(model)
{
    insets(Insetsi(8));
    flags(Element::FILL);

    auto icon_and_text = add<Widget::Element>();
    icon_and_text->add(Widget::icon("chip"));
    icon_and_text->add(Widget::label("Memory"));

    auto cpu_filler = add<Widget::Element>();
    cpu_filler->flags(Element::FILL);

    _label_usage = Widget::label("Usage: nil Mio", Anchor::RIGHT);
    add(_label_usage);

    _label_available = Widget::label("Available: nil Mio", Anchor::RIGHT);
    add(_label_available);

    _label_greedy = Widget::label("Most greedy: nil", Anchor::RIGHT);
    add(_label_greedy);

    _graph_timer = own<Async::Timer>(500, [&]() {
        SystemStatus status{};
        hj_system_status(&status);

        record(status.used_ram / (float)status.total_ram);
    });

    _graph_timer->start();

    _text_timer = own<Async::Timer>(1000, [&]() {
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