#include <libsystem/eventloop/Timer.h>
#include <libsystem/io/Path.h>
#include <libsystem/system/System.h>
#include <libsystem/utils/BufferBuilder.h>
#include <libwidget/Application.h>
#include <libwidget/Widgets.h>
#include <libwidget/dialog/Dialog.h>

#include "task-manager/CPUGraph.h"
#include "task-manager/RAMGraph.h"
#include "task-manager/TaskModel.h"

struct TaskManagerWindow : public Window
{
    /// --- Graphs --- ///
    RAMGraph *ram_graph;
    CPUGraph *cpu_graph;

    /// --- Table view --- //
    Table *table;
    TaskModel *table_model;
    RefPtr<Timer> table_timer;
};

void widget_table_update(TaskManagerWindow *window)
{
    model_update((Model *)window->table_model);
    window->table->should_repaint();
    window->table->should_relayout();
}

void task_manager_window_destroy(TaskManagerWindow *window)
{
    model_destroy((Model *)window->table_model);
}

int main(int argc, char **argv)
{
    application_initialize(argc, argv);

    TaskManagerWindow *window = __create(TaskManagerWindow);

    window_initialize(window, WINDOW_RESIZABLE);

    window->icon(Icon::get("memory"));
    window->title("Task Manager");
    window->size(Vec2i(700, 500));

    window->root()->layout(VFLOW(0));

    /// --- Toolbar --- ///
    auto toolbar = toolbar_create(window->root());
    new Button(toolbar, BUTTON_FILLED, Icon::get("plus"), "New task");

    auto cancel_task_button = new Button(toolbar, BUTTON_TEXT, Icon::get("close"), "Cancel task");
    cancel_task_button->on(Event::ACTION, [](auto) {
        dialog_message(Icon::get("close"), "Cancel task", "Are you sure about that ?", DIALOG_BUTTON_YES | DIALOG_BUTTON_NO);
    });

    /// --- Table view --- //
    window->table_model = task_model_create();

    window->table = new Table(window->root(), window->table_model);
    window->table->attributes(LAYOUT_FILL);
    window->table_timer = make<Timer>(1000, [&]() { widget_table_update(window); });
    window->table_timer->start();

    /// --- Graphs --- ///
    auto graphs_container = new Panel(window->root());
    graphs_container->layout(HFLOW(0));
    graphs_container->max_height(96);

    window->cpu_graph = new CPUGraph(graphs_container, window->table_model);

    new Separator(graphs_container);

    window->ram_graph = new RAMGraph(graphs_container, window->table_model);

    window->show();

    return application_run();
}
