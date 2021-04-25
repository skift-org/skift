#include <libwidget/Button.h>
#include <libwidget/Elements.h>

#include <libwidget/TitleBar.h>
#include <libwidget/dialog/MessageBox.h>

#include "task-manager/windows/MainWindow.h"

namespace task_manager
{

MainWinow::MainWinow() : Window(WINDOW_RESIZABLE)
{
    size(Math::Vec2i(700, 500));

    root()->layout(VFLOW(0));

    root()->add<Widget::TitleBar>(
        Graphic::Icon::get("memory"),
        "Task Manager");

    /// --- Toolbar --- ///
    auto toolbar = root()->add(Widget::panel());

    toolbar->layout(HFLOW(4));
    toolbar->insets(Insetsi(4, 4));

    toolbar->add<Widget::Button>(Widget::Button::FILLED, Graphic::Icon::get("plus"), "New task");

    auto cancel_task_button = toolbar->add<Widget::Button>(Widget::Button::TEXT, Graphic::Icon::get("close"), "Cancel task");
    cancel_task_button->on(Widget::Event::ACTION, [&](auto) {
        auto result = Widget::MessageBox::create_and_show(
            "Cancel task",
            "Are you sure about that ?",
            Graphic::Icon::get("close"),
            Widget::DialogButton::YES | Widget::DialogButton::NO);

        if (result == Widget::DialogResult::YES)
        {
            _table_model->kill_task(_table->selected());
        };
    });

    /// --- Table view --- //
    _table_model = make<TaskModel>();

    _table = root()->add<Widget::Table>(_table_model);
    _table->flags(Widget::Element::FILL);

    _table_timer = own<Async::Timer>(1000, [&]() {
        _table_model->update();
    });

    _table_timer->start();

    /// --- Graphs --- ///
    auto graphs_container = root()->add(Widget::panel());
    graphs_container->layout(HFLOW(0));
    graphs_container->min_height(128);

    _cpu_graph = graphs_container->add<CPUGraph>(_table_model);

    graphs_container->add(Widget::separator());

    _ram_graph = graphs_container->add<RAMGraph>(_table_model);
}

} // namespace task_manager
