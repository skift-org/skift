#include <abi/Syscalls.h>

#include <libio/Format.h>
#include <libsystem/system/System.h>
#include <libutils/StringBuilder.h>
#include <libwidget/Container.h>
#include <libwidget/IconPanel.h>

#include "task-manager/widgets/RAMGraph.h"

namespace task_manager
{

RAMGraph::RAMGraph(RefPtr<TaskModel> model)
    : Graph(256, Graphic::Colors::ROYALBLUE),
      _model(model)
{
    layout(VFLOW(0));
    insets(Insetsi(8));
    flags(Element::FILL);

    auto icon_and_text = add<Widget::Container>();
    icon_and_text->layout(HFLOW(4));
    icon_and_text->add<Widget::IconPanel>(Graphic::Icon::get("chip"));
    icon_and_text->add<Widget::Label>("Memory");

    auto cpu_filler = add<Widget::Container>();
    cpu_filler->flags(Element::FILL);

    _label_usage = add<Widget::Label>("Usage: nil Mio", Anchor::RIGHT);
    _label_available = add<Widget::Label>("Available: nil Mio", Anchor::RIGHT);
    _label_greedy = add<Widget::Label>("Most greedy: nil", Anchor::RIGHT);

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