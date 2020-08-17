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
    Label *cpu_uptime;

    /// --- Table view --- //
    Table *table;
    TaskModel *table_model;
    Timer *table_timer;
};

void widget_ram_update(TaskManagerWindow *window)
{
    SystemStatus status = system_get_status();
    window->ram_graph->record(status.used_ram / (float)status.total_ram);

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

    window->ram_usage->text(buffer_usage);

    window->ram_avaliable->text(buffer_avaliable);

    window->ram_greedy->text(buffer_builder_intermediate(buffer_greedy));

    // Destroy buffer
    buffer_builder_destroy(buffer_greedy);
}

void widget_cpu_update(TaskManagerWindow *window)
{
    SystemStatus status = system_get_status();

    window->cpu_graph->record(status.cpu_usage / 100.0);

    const char *greedy = task_model_get_greedy_process(window->table_model, 1);
    int percentage = (int)status.cpu_usage;

    char buffer_average[50];
    snprintf(buffer_average, 200, "Average: %i%%", percentage);
    window->cpu_average->text(buffer_average);

    auto buffer_greedy = buffer_builder_create(24);
    buffer_builder_append_str(buffer_greedy, "Most greedy: ");
    buffer_builder_append_str(buffer_greedy, greedy);
    window->cpu_greedy->text(buffer_builder_intermediate(buffer_greedy));
    buffer_builder_destroy(buffer_greedy);

    ElapsedTime seconds = status.uptime;
    int days = seconds / 86400;
    seconds %= 86400;
    int hours = seconds / 3600;
    seconds %= 3600;
    int minutes = seconds / 60;
    seconds %= 60;

    char buffer_uptime[50];
    snprintf(buffer_uptime, 50, "Uptime: %3d:%02d:%02d:%02d", days, hours, minutes, seconds);
    window->cpu_uptime->text(buffer_uptime);
}

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

    window_root(window)->layout(VFLOW(0));

    /// --- Toolbar --- ///
    auto toolbar = toolbar_create(window_root(window));
    new Button(toolbar, BUTTON_FILLED, Icon::get("plus"), "New task");

    auto cancel_task_button = new Button(toolbar, BUTTON_TEXT, Icon::get("close"), "Cancel task");
    cancel_task_button->on(Event::ACTION, [](auto) {
        dialog_message(Icon::get("close"), "Cancel task", "Are you sure about that ?", DIALOG_BUTTON_YES | DIALOG_BUTTON_NO);
    });

    /// --- Table view --- //
    window->table_model = task_model_create();

    window->table = new Table(window_root(window), window->table_model);
    window->table->attributes(LAYOUT_FILL);
    window->table_timer = timer_create(window, 1000, (TimerCallback)widget_table_update);
    timer_start(window->table_timer);

    /// --- Graphs --- ///
    auto graphs_container = new Panel(window_root(window));
    graphs_container->layout(HFLOW(0));
    graphs_container->max_height(96);

    window->cpu_graph = new Graph(graphs_container, 256, COLOR_SEAGREEN);
    window->cpu_graph->layout(VFLOW(0));
    window->cpu_graph->insets(Insets(8));
    window->cpu_graph->attributes(LAYOUT_FILL);

    auto cpu_icon_and_text = new Container(window->cpu_graph);
    cpu_icon_and_text->layout(HFLOW(4));
    new IconPanel(cpu_icon_and_text, Icon::get("memory"));
    new Label(cpu_icon_and_text, "Processor");

    auto cpu_filler = new Container(window->cpu_graph);
    cpu_filler->attributes(LAYOUT_FILL);

    window->cpu_average = new Label(window->cpu_graph, "Average: nil%", Position::RIGHT);
    window->cpu_greedy = new Label(window->cpu_graph, "Most greedy: nil", Position::RIGHT);
    window->cpu_uptime = new Label(window->cpu_graph, "Uptime: nil", Position::RIGHT);

    window->cpu_timer = timer_create(window, 100, (TimerCallback)widget_cpu_update);
    timer_start(window->cpu_timer);

    new Separator(graphs_container);

    window->ram_graph = new Graph(graphs_container, 256, COLOR_ROYALBLUE);
    window->ram_graph->layout(VFLOW(0));
    window->ram_graph->insets(Insets(8));
    window->ram_graph->attributes(LAYOUT_FILL);

    auto ram_icon_and_text = new Container(window->ram_graph);
    ram_icon_and_text->layout(HFLOW(4));
    new IconPanel(ram_icon_and_text, Icon::get("chip"));
    new Label(ram_icon_and_text, "Memory");

    auto ram_filler = new Container(window->ram_graph);
    ram_filler->attributes(LAYOUT_FILL);

    window->ram_usage = new Label(window->ram_graph, "Usage: nil Mio", Position::RIGHT);
    window->ram_avaliable = new Label(window->ram_graph, "Avaliable: nil Mio", Position::RIGHT);
    window->ram_greedy = new Label(window->ram_graph, "Most greedy: nil", Position::RIGHT);

    window->ram_timer = timer_create(window, 500, (TimerCallback)widget_ram_update);
    timer_start(window->ram_timer);

    window_show(window);

    return application_run();
}
