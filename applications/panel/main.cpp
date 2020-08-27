#include <libsystem/core/CString.h>
#include <libsystem/eventloop/Timer.h>
#include <libsystem/io/Path.h>
#include <libsystem/process/Process.h>
#include <libsystem/system/System.h>
#include <libwidget/Application.h>
#include <libwidget/Menu.h>
#include <libwidget/Screen.h>
#include <libwidget/Widgets.h>

void widget_cpu_update(Graph *widget)
{
    SystemStatus status = system_get_status();
    widget->record(status.cpu_usage / 100.0);
}

int main(int argc, char **argv)
{
    application_initialize(argc, argv);

    Window *window = window_create(WINDOW_BORDERLESS | WINDOW_ALWAYS_FOCUSED);

    window->title("Panel");
    window->type(WINDOW_TYPE_PANEL);
    window->bound(screen_get_bound().take_top(36));
    window->on(Event::DISPLAY_SIZE_CHANGED, [&](auto event) {
        window->bound(window_bound(window).with_width(event->display.size.x()));
    });

    window_root(window)->layout(VFLOW(0));

    auto panel_container = new Container(window_root(window));
    panel_container->attributes(LAYOUT_FILL);
    panel_container->layout(HFLOW(8));
    panel_container->insets(Insets(4));

    new Separator(window_root(window));

    auto menu = new Button(panel_container, BUTTON_TEXT, Icon::get("menu"), "Applications");
    menu->on(Event::ACTION, [](auto) { process_run("menu", nullptr); });

    auto widget_date_and_time = new Label(panel_container, "", Position::CENTER);
    widget_date_and_time->attributes(LAYOUT_FILL);

    auto graph_container = new Panel(panel_container);
    graph_container->layout(VGRID(1));

    auto ram_graph = new Graph(graph_container, 50, COLOR_ROYALBLUE);
    new Label(ram_graph, "RAM", Position::CENTER);

    auto cpu_graph = new Graph(graph_container, 50, COLOR_SEAGREEN);
    new Label(cpu_graph, "CPU", Position::CENTER);

    new Label(panel_container, "user");

    auto clock_timer = make<Timer>(500, [&]() {
        TimeStamp timestamp = timestamp_now();
        DateTime datetime = timestamp_to_datetime(timestamp);

        char buffer[256];
        snprintf(buffer, 256, "%02d:%02d:%02d ", datetime.hour, datetime.minute, datetime.second);
        widget_date_and_time->text(buffer);
    });

    clock_timer->start();

    auto ram_timer = make<Timer>(500, [&]() {
        SystemStatus status = system_get_status();
        ram_graph->record(status.used_ram / (float)status.total_ram);
    });

    ram_timer->start();

    auto cpu_timer = make<Timer>(100, [&]() {
        SystemStatus status = system_get_status();
        cpu_graph->record(status.cpu_usage / 100.0);
    });

    cpu_timer->start();

    graph_container->on(Event::MOUSE_BUTTON_PRESS, [](auto) { process_run("task-manager", nullptr); });

    window_show(window);

    return application_run();
}
