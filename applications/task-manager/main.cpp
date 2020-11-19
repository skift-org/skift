#include <libsystem/eventloop/Timer.h>
#include <libsystem/system/System.h>
#include <libutils/Path.h>
#include <libwidget/Application.h>
#include <libwidget/Widgets.h>
#include <libwidget/dialog/Dialog.h>

#include "task-manager/CPUGraph.h"
#include "task-manager/RAMGraph.h"
#include "task-manager/TaskModel.h"

class TaskManagerWindow : public Window
{
private:
    RAMGraph *_ram_graph;
    CPUGraph *_cpu_graph;
    Table *_table;
    RefPtr<TaskModel> _table_model;
    OwnPtr<Timer> _table_timer;

public:
    TaskManagerWindow() : Window(WINDOW_RESIZABLE)
    {
        icon(Icon::get("memory"));
        title("Task Manager");
        size(Vec2i(700, 500));

        root()->layout(VFLOW(0));

        /// --- Toolbar --- ///
        auto toolbar = toolbar_create(root());
        new Button(toolbar, BUTTON_FILLED, Icon::get("plus"), "New task");

        auto cancel_task_button = new Button(toolbar, BUTTON_TEXT, Icon::get("close"), "Cancel task");
        cancel_task_button->on(Event::ACTION, [&](auto) {
            if (dialog_message(Icon::get("close"), "Cancel task", "Are you sure about that ?", DIALOG_BUTTON_YES | DIALOG_BUTTON_NO) == DIALOG_BUTTON_YES)
            {
                _table_model->kill_task(_table->selected());
            };
        });

        /// --- Table view --- //
        _table_model = make<TaskModel>();

        _table = new Table(root(), _table_model);
        _table->attributes(LAYOUT_FILL);

        _table_timer = own<Timer>(1000, [&]() {
            _table_model->update();
        });

        _table_timer->start();
        /// --- Graphs --- ///
        auto graphs_container = new Panel(root());
        graphs_container->layout(HFLOW(0));
        graphs_container->max_height(96);

        _cpu_graph = new CPUGraph(graphs_container, _table_model);

        new Separator(graphs_container);

        _ram_graph = new RAMGraph(graphs_container, _table_model);
    }
};

int main(int argc, char **argv)
{
    application_initialize(argc, argv);

    auto window = new TaskManagerWindow();
    window->show();

    return application_run();
}
