#include <libsystem/cstring.h>
#include <libsystem/eventloop/Timer.h>
#include <libsystem/process/Launchpad.h>
#include <libsystem/system.h>
#include <libwidget/Application.h>
#include <libwidget/Widgets.h>

void widget_date_and_time_update(Widget *widget)
{
    TimeStamp timestamp = timestamp_now();
    DateTime datetime = timestamp_to_datetime(timestamp);

    char buffer[256];

    snprintf(buffer, 256, "%02d:%02d:%02d ", datetime.hour, datetime.minute, datetime.second);

    label_set_text(widget, buffer);
}

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

void open_menu()
{
    Launchpad *menu = launchpad_create("menu", "/bin/menu");
    launchpad_launch(menu, NULL);
}

void open_task_manager()
{
    Launchpad *task_manager = launchpad_create("task-manager", "/bin/task-manager");
    launchpad_launch(task_manager, NULL);
}

int main(int argc, char **argv)
{
    application_initialize(argc, argv);

    Window *window = window_create(NULL, "Panel", 1024, 36, WINDOW_BORDERLESS | WINDOW_ALWAYS_FOCUSED);

    window_root(window)->layout = HFLOW(8);
    window_root(window)->insets = INSETS(4);

    Widget *menu = button_create_with_icon_and_text(window_root(window), "menu", "skiftOS");
    widget_set_event_handler(menu, EVENT_ACTION, EVENT_HANDLER(NULL, open_menu));

    Widget *widget_date_and_time = label_create(window_root(window), "");
    widget_date_and_time->layout_attributes = LAYOUT_FILL;

    Widget *graph_container = container_create(window_root(window));
    graph_container->layout = VGRID(1);

    Widget *ram_graph = graph_create(graph_container, 50, COLOR_ROYALBLUE);
    label_create(ram_graph, "RAM");

    Widget *cpu_graph = graph_create(graph_container, 50, COLOR_SEAGREEN);
    label_create(cpu_graph, "CPU");

    label_create(window_root(window), "user");

    timer_start(timer_create(widget_date_and_time, 500, (TimerCallback)widget_date_and_time_update));
    timer_start(timer_create(ram_graph, 500, (TimerCallback)widget_ram_update));
    timer_start(timer_create(cpu_graph, 100, (TimerCallback)widget_cpu_update));

    widget_set_event_handler(graph_container, EVENT_MOUSE_BUTTON_PRESS, EVENT_HANDLER(NULL, open_task_manager));

    window_show(window);

    return application_run();
}
