#include <libwidget/Button.h>
#include <libwidget/Panel.h>
#include <libwidget/Separator.h>
#include <libwidget/TitleBar.h>
#include <libwidget/dialog/MessageBox.h>

#include "task-manager/windows/MainWindow.h"

namespace task_manager
{

MainWinow::MainWinow() : Window(WINDOW_RESIZABLE)
{
    icon(Graphic::Icon::get("memory"));
    title("Task Manager");
    size(Math::Vec2i(700, 500));

    root()->layout(VFLOW(0));

    new Widget::TitleBar(root());

    /// --- Toolbar --- ///
    auto toolbar = new Widget::Panel(root());

    toolbar->layout(HFLOW(4));
    toolbar->insets(Insetsi(4, 4));
    toolbar->max_height(38);
    toolbar->min_height(38);

    new Widget::Button(toolbar, Widget::Button::FILLED, Graphic::Icon::get("plus"), "New task");

    auto cancel_task_button = new Widget::Button(toolbar, Widget::Button::TEXT, Graphic::Icon::get("close"), "Cancel task");
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

    _table = new Widget::Table(root(), _table_model);
    _table->flags(Widget::Component::FILL);

    _table_timer = own<Timer>(1000, [&]() {
        _table_model->update();
    });

    _table_timer->start();

    /// --- Graphs --- ///
    auto graphs_container = new Widget::Panel(root());
    graphs_container->layout(HFLOW(0));
    graphs_container->max_height(96);

    _cpu_graph = new CPUGraph(graphs_container, _table_model);

    new Widget::Separator(graphs_container);

    _ram_graph = new RAMGraph(graphs_container, _table_model);
}

} // namespace task_manager
