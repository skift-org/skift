#include <libsystem/eventloop/Timer.h>
#include <libsystem/io/Path.h>
#include <libsystem/system/System.h>
#include <libwidget/Application.h>
#include <libwidget/Widgets.h>
#include <libwidget/dialog/Dialog.h>

#include "task-manager/TaskModel.h"

struct TaskManagerWindow
{
    Window window;

    /// --- Graphs --- ///
    Widget *ram_graph;
    Timer *ram_timer;
    Widget *cpu_graph;
    Timer *cpu_timer;

    // --- CPU/RAM stats --- //
    Widget *ram_usage;
    Widget *ram_avaliable;
    Widget *ram_greedy;
    Widget *cpu_average;
    Widget *cpu_greedy;

    /// --- Table view --- //
    Widget *table;
    TaskModel *table_model;
    Timer *table_timer;
};

const char *get_greedy_process(TaskModel *model, int ram_cpu)
{
    const char *greedy = "";

    int row_count = json_array_length(model->data);
    int list[row_count];
    for (int row = 0; row < row_count; row++)
    {
        // 0 means memory. 1 means processor
        if (ram_cpu == 0)
        {
            JsonValue *task = json_array_get(model->data, (size_t)row);
            list[row] = json_integer_value(json_object_get(task, "ram"));
        }
        else if (ram_cpu == 1)
        {
            JsonValue *task = json_array_get(model->data, (size_t)row);
            list[row] = json_integer_value(json_object_get(task, "cpu"));
        }
    }
    int greedy_index = 0;
    for (int i = 0; i < row_count; i++)
    {
        if (list[greedy_index] < list[i])
        {
            greedy_index = i;
        }
    }
    JsonValue *task = json_array_get(model->data, (size_t)greedy_index);
    greedy = json_string_value(json_object_get(task, "name"));
    return greedy;
}

void widget_ram_update(TaskManagerWindow *window)
{
    SystemStatus status = system_get_status();

    graph_record((Graph *)window->ram_graph, status.used_ram / (double)status.total_ram);

    char buffer1[200];
    char buffer2[200];
    // This not ideal, when dealing with program names longer than 400 characters. Hopefully that shouldn't happen
    char buffer3[400];

    int usage = (int)status.used_ram / 1024 / 1024;
    int avaliable = (int)status.total_ram / 1024 / 1024;
    const char *greedy = get_greedy_process(window->table_model, 0);

    snprintf(buffer1, 200, "Usage: %i Mio", usage);
    snprintf(buffer2, 200, "Avaliable: %i Mio", avaliable);
    snprintf(buffer3, 400, "Most greedy: %s", greedy);

    label_set_text(window->ram_usage, buffer1);
    label_set_text(window->ram_avaliable, buffer2);
    label_set_text(window->ram_greedy, buffer3);
}

void widget_cpu_update(TaskManagerWindow *window)
{
    SystemStatus status = system_get_status();

    graph_record((Graph *)window->cpu_graph, status.cpu_usage / 100.0);

    char buffer1[200];
    char buffer2[400];

    const char *greedy = get_greedy_process(window->table_model, 1);
    int percentage = (int)status.cpu_usage;
    snprintf(buffer1, 200, "Average: %i%%", percentage);
    snprintf(buffer2, 400, "Most greedy: %s", greedy);
    label_set_text(window->cpu_average, buffer1);
    label_set_text(window->cpu_greedy, buffer2);
}

void widget_table_update(TaskManagerWindow *window)
{
    model_update((Model *)window->table_model);
    widget_update(window->table);
    widget_layout(window->table);
}

void task_manager_window_destroy(TaskManagerWindow *window)
{
    model_destroy((Model *)window->table_model);
}

void cancel_task()
{
    dialog_message(Icon::get("close"), "Cancel task", "Are you sure about that ?", DIALOG_BUTTON_OK);
}

int main(int argc, char **argv)
{
    application_initialize(argc, argv);

    TaskManagerWindow *window = __create(TaskManagerWindow);

    window_initialize((Window *)window, WINDOW_RESIZABLE);

    window_set_icon((Window *)window, Icon::get("memory"));
    window_set_title((Window *)window, "Task Manager");
    window_set_size((Window *)window, Vec2i(700, 500));

    window_root((Window *)window)->layout = VFLOW(0);

    /// --- Toolbar --- ///
    Widget *toolbar = toolbar_create(window_root((Window *)window));
    Widget *new_task_button = button_create_with_icon_and_text(toolbar, BUTTON_FILLED, Icon::get("plus"), "New task");
    __unused(new_task_button);

    Widget *cancel_task_button = button_create_with_icon_and_text(toolbar, BUTTON_TEXT, Icon::get("close"), "Cancel task");
    widget_set_event_handler(cancel_task_button, EVENT_ACTION, EVENT_HANDLER(nullptr, cancel_task));

    /// --- Table view --- //
    window->table_model = task_model_create();

    window->table = table_create(window_root((Window *)window), (Model *)window->table_model);
    window->table->layout_attributes = LAYOUT_FILL;
    window->table_timer = timer_create(window, 1000, (TimerCallback)widget_table_update);
    timer_start(window->table_timer);

    /// --- Graphs --- ///
    Widget *graphs_container = panel_create(window_root((Window *)window));
    graphs_container->layout = HFLOW(0);
    graphs_container->max_height = 96;

    window->cpu_graph = graph_create(graphs_container, 256, COLOR_SEAGREEN);
    window->cpu_graph->layout = VFLOW(0);
    window->cpu_graph->insets = Insets(8);
    window->cpu_graph->layout_attributes = LAYOUT_FILL;

    Widget *cpu_icon_and_text = container_create(window->cpu_graph);
    cpu_icon_and_text->layout = HFLOW(4);
    icon_panel_create(cpu_icon_and_text, Icon::get("memory"));
    label_create(cpu_icon_and_text, "Processor");

    window->cpu_timer = timer_create(window, 100, (TimerCallback)widget_cpu_update);
    timer_start(window->cpu_timer);

    separator_create(graphs_container);

    window->ram_graph = graph_create(graphs_container, 256, COLOR_ROYALBLUE);
    window->ram_graph->layout = VFLOW(0);
    window->ram_graph->insets = Insets(8);
    window->ram_graph->layout_attributes = LAYOUT_FILL;

    Widget *ram_icon_and_text = container_create(window->ram_graph);
    ram_icon_and_text->layout = HFLOW(4);
    icon_panel_create(ram_icon_and_text, Icon::get("chip"));
    label_create(ram_icon_and_text, "Memory");

    container_create(window->ram_graph)->layout_attributes = LAYOUT_FILL;
    window->ram_usage = label_create(window->ram_graph, "Usage: nil Mio");
    label_set_text_position(window->ram_usage, Position::RIGHT);
    window->ram_avaliable = label_create(window->ram_graph, "Avaliable: nil Mio");
    label_set_text_position(window->ram_avaliable, Position::RIGHT);
    window->ram_greedy = label_create(window->ram_graph, "Most greedy: nil");
    label_set_text_position(window->ram_greedy, Position::RIGHT);

    container_create(window->cpu_graph)->layout_attributes = LAYOUT_FILL;
    window->cpu_average = label_create(window->cpu_graph, "Average: nil%");
    label_set_text_position(window->cpu_average, Position::BOTTOM_RIGHT);
    window->cpu_greedy = label_create(window->cpu_graph, "Most greedy: the compositor eats a lot");
    label_set_text_position(window->cpu_greedy, Position::BOTTOM_RIGHT);

    window->ram_timer = timer_create(window, 500, (TimerCallback)widget_ram_update);
    timer_start(window->ram_timer);

    window_show((Window *)window);

    return application_run();
}
