#include <libsystem/eventloop/Timer.h>
#include <libsystem/system.h>
#include <libwidget/Application.h>
#include <libwidget/Container.h>
#include <libwidget/Graph.h>
#include <libwidget/Table.h>

#include "task-manager/TaskModel.h"

typedef struct
{
    Window window;

    /// --- Graphs --- ///
    Widget *ram_graph;
    Timer *ram_timer;
    Widget *cpu_graph;
    Timer *cpu_timer;

    /// --- Table view --- //
    Widget *table;
    TaskModel *table_model;
    Timer *table_timer;
} TaskManagerWindow;

void widget_ram_update(Graph *widget)
{
    SystemStatus status = system_get_status();

    graph_record(widget, status.used_ram / (double)status.total_ram);
}

void widget_cpu_update(Graph *widget)
{
    SystemStatus status = system_get_status();

    graph_record(widget, status.cpu_usage / 100.0);
}

void widget_table_update(TaskManagerWindow *window)
{
    model_update((Model *)window->table_model);
    widget_update(window->table);
}

void task_manager_window_destroy(TaskManagerWindow *window)
{
    model_destroy((Model *)window->table_model);
}

int main(int argc, char **argv)
{
    application_initialize(argc, argv);

    TaskManagerWindow *window = __create(TaskManagerWindow);
    window_initialize((Window *)window, "graph", "Task Manager", 500, 400, WINDOW_RESIZABLE);
    window_root((Window *)window)->layout = (Layout){LAYOUT_VFLOW, 0, 0};

    /// --- Graphs --- ///
    Widget *graphs_container = container_create(window_root((Window *)window));
    graphs_container->layout = (Layout){LAYOUT_HGRID, 8, 0};

    window->ram_graph = graph_create(graphs_container, 100, COLOR_ROYALBLUE);
    window->ram_timer = timer_create(window->ram_graph, 500, (TimerCallback)widget_ram_update);
    timer_start(window->ram_timer);

    window->cpu_graph = graph_create(graphs_container, 500, COLOR_SEAGREEN);
    window->cpu_timer = timer_create(window->cpu_graph, 100, (TimerCallback)widget_cpu_update);
    timer_start(window->cpu_timer);

    /// --- Table view --- //
    window->table_model = task_model_create();

    window->table = table_create(window_root((Window *)window), (Model *)window->table_model);
    window->table->layout_attributes = LAYOUT_FILL;
    window->table_timer = timer_create(window, 1000, (TimerCallback)widget_table_update);
    timer_start(window->table_timer);

    window_show((Window *)window);

    return application_run();
}
