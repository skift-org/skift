#include <libsystem/eventloop/Timer.h>
#include <libsystem/io/Path.h>
#include <libsystem/system/System.h>
#include <libsystem/utils/BufferBuilder.h>
#include <libwidget/Application.h>
#include <libwidget/Widgets.h>
#include <libwidget/dialog/Dialog.h>

#include "task-manager/TaskModel.h"

struct TaskManagerWindow : public Window
{
    /// --- Graphs --- ///
    Graph *ram_graph;
    Timer *ram_timer;
    Graph *cpu_graph;
    Timer *cpu_timer;

    // --- CPU/RAM stats --- //
    Label *ram_usage;
    Label *ram_avaliable;
    Label *ram_greedy;
    Label *cpu_average;
    Label *cpu_greedy;

    /// --- Table view --- //
    Table *table;
    TaskModel *table_model;
    Timer *table_timer;
};

void widget_ram_update(TaskManagerWindow *window)
{
    SystemStatus status = system_get_status();

    graph_record(window->ram_graph, status.used_ram / (double)status.total_ram);

    // Stats
    int usage = (int)status.used_ram / 1024 / 1024;
    int avaliable = (int)status.total_ram / 1024 / 1024;
    const char *greedy = task_model_get_greedy_process(window->table_model, 0);

    char buffer_usage[50];
    char buffer_avaliable[50];

    size_t buff_greedy_size = strlen("Most greedy: ") + strlen(greedy);
    auto buffer_greedy = buffer_builder_create(buff_greedy_size);

    buffer_builder_append_str(buffer_greedy, "Most greedy: ");
    buffer_builder_append_str(buffer_greedy, greedy);

    snprintf(buffer_usage, 50, "Usage: %i Mio", usage);
    snprintf(buffer_avaliable, 50, "Avaliable: %i Mio", avaliable);

    label_set_text(window->ram_usage, buffer_usage);

    label_set_text(window->ram_avaliable, buffer_avaliable);

    label_set_text(window->ram_greedy, buffer_builder_intermediate(buffer_greedy));

    // Destroy buffer
    buffer_builder_destroy(buffer_greedy);
}

void widget_cpu_update(TaskManagerWindow *window)
{
    SystemStatus status = system_get_status();

    graph_record(window->cpu_graph, status.cpu_usage / 100.0);

    const char *greedy = task_model_get_greedy_process(window->table_model, 1);
    int percentage = (int)status.cpu_usage;

    char buffer_average[50];

    size_t buff_greedy_size = strlen("Most greedy: ") + strlen(greedy);
    auto buffer_greedy = buffer_builder_create(buff_greedy_size);

    buffer_builder_append_str(buffer_greedy, "Most greedy: ");
    buffer_builder_append_str(buffer_greedy, greedy);

    snprintf(buffer_average, 200, "Average: %i%%", percentage);

    label_set_text(window->cpu_average, buffer_average);
    label_set_text(window->cpu_greedy, buffer_builder_intermediate(buffer_greedy));

    // Destroy
    buffer_builder_destroy(buffer_greedy);
}

void widget_table_update(TaskManagerWindow *window)
{
    model_update((Model *)window->table_model);
    window->table->should_repaint();
    widget_layout(window->table);
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

    window_set_icon(window, Icon::get("memory"));
    window_set_title(window, "Task Manager");
    window_set_size(window, Vec2i(700, 500));

    window_root(window)->layout = VFLOW(0);

    /// --- Toolbar --- ///
    Widget *toolbar = toolbar_create(window_root(window));
    Widget *new_task_button = button_create_with_icon_and_text(toolbar, BUTTON_FILLED, Icon::get("plus"), "New task");
    __unused(new_task_button);

    Widget *cancel_task_button = button_create_with_icon_and_text(toolbar, BUTTON_TEXT, Icon::get("close"), "Cancel task");
    cancel_task_button->on(Event::ACTION, [](auto) {
        dialog_message(Icon::get("close"), "Cancel task", "Are you sure about that ?", DIALOG_BUTTON_YES | DIALOG_BUTTON_NO);
    });

    /// --- Table view --- //
    window->table_model = task_model_create();

    window->table = table_create(window_root(window), (Model *)window->table_model);
    window->table->layout_attributes = LAYOUT_FILL;
    window->table_timer = timer_create(window, 1000, (TimerCallback)widget_table_update);
    timer_start(window->table_timer);

    /// --- Graphs --- ///
    Widget *graphs_container = panel_create(window_root(window));
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

    window_show(window);

    return application_run();
}
