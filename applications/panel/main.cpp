#include <libsystem/core/CString.h>
#include <libsystem/eventloop/Timer.h>
#include <libsystem/io/Path.h>
#include <libsystem/process/Process.h>
#include <libsystem/system/System.h>
#include <libwidget/Application.h>
#include <libwidget/Menu.h>
#include <libwidget/Screen.h>
#include <libwidget/Widgets.h>

void widget_date_and_time_update(Label *widget)
{
    TimeStamp timestamp = timestamp_now();
    DateTime datetime = timestamp_to_datetime(timestamp);

    char buffer[256];
    snprintf(buffer, 256, "%02d:%02d:%02d ", datetime.hour, datetime.minute, datetime.second);
    widget->update_text(buffer);
}

void widget_ram_update(Graph *widget)
{
    SystemStatus status = system_get_status();
    widget->record(status.used_ram / (double)status.total_ram);
}

void widget_cpu_update(Graph *widget)
{
    SystemStatus status = system_get_status();
    widget->record(status.cpu_usage / 100.0);
}

int main(int argc, char **argv)
{
    application_initialize(argc, argv);

    Window *window = window_create(WINDOW_BORDERLESS | WINDOW_ALWAYS_FOCUSED);

    window_set_title(window, "Panel");
    window_set_bound(window, screen_get_bound().take_top(36));

    window_root(window)->layout = HFLOW(8);
    window_root(window)->insets = Insets(4);

    Widget *menu = new Button(window_root(window), BUTTON_TEXT, Icon::get("menu"), "Applications");
    menu->on(Event::ACTION, [](auto) { process_run("menu", nullptr); });

    auto widget_date_and_time = new Label(window_root(window), "", Position::CENTER);
    widget_date_and_time->layout_attributes = LAYOUT_FILL;

    Widget *graph_container = new Panel(window_root(window));
    graph_container->layout = VGRID(1);

    Widget *ram_graph = new Graph(graph_container, 50, COLOR_ROYALBLUE);
    new Label(ram_graph, "RAM", Position::CENTER);

    Widget *cpu_graph = new Graph(graph_container, 50, COLOR_SEAGREEN);
    new Label(cpu_graph, "CPU", Position::CENTER);

    new Label(window_root(window), "user");

    timer_start(timer_create(widget_date_and_time, 500, (TimerCallback)widget_date_and_time_update));
    timer_start(timer_create(ram_graph, 500, (TimerCallback)widget_ram_update));
    timer_start(timer_create(cpu_graph, 100, (TimerCallback)widget_cpu_update));

    graph_container->on(Event::MOUSE_BUTTON_PRESS, [](auto) { process_run("task-manager", nullptr); });

    window_show(window);

    return application_run();
}
