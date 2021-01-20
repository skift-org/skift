#include <abi/Syscalls.h>

#include <libsystem/core/CString.h>
#include <libsystem/eventloop/Timer.h>
#include <libsystem/process/Environment.h>
#include <libsystem/process/Process.h>
#include <libsystem/system/System.h>

#include <libwidget/Application.h>
#include <libwidget/Menu.h>
#include <libwidget/Screen.h>
#include <libwidget/Widgets.h>

static constexpr int PANEL_HEIGHT = 38;

int main(int argc, char **argv)
{
    Application::initialize(argc, argv);

    Window *window = new Window(WINDOW_BORDERLESS | WINDOW_ALWAYS_FOCUSED | WINDOW_ACRYLIC);

    window->title("Panel");
    window->type(WINDOW_TYPE_PANEL);
    window->bound(Screen::bound().take_top(PANEL_HEIGHT));
    window->opacity(0.85);
    window->on(Event::DISPLAY_SIZE_CHANGED, [&](auto) {
        window->bound(Screen::bound().take_top(PANEL_HEIGHT));
    });

    window->root()->layout(VFLOW(0));

    auto panel_container = new Container(window->root());
    panel_container->flags(Widget::FILL);
    panel_container->layout(HFLOW(8));
    panel_container->insets(Insetsi(4));

    new Separator(window->root());
    (new Separator(window->root()))->color(THEME_BORDER, Colors::BLACK.with_alpha(0.25));

    auto menu = new Button(panel_container, Button::TEXT, Icon::get("menu"), "Applications");
    menu->on(Event::ACTION, [](auto) { process_run("menu", nullptr); });

    new Spacer(panel_container);

    auto widget_date_and_time = new Label(panel_container, "", Anchor::CENTER);

    new Spacer(panel_container);

    auto graph_container = new Button(panel_container, Button::TEXT);
    graph_container->layout(VGRID(1));
    graph_container->insets(0);

    auto ram_graph = new Graph(graph_container, 50, Colors::ROYALBLUE);
    new Label(ram_graph, "RAM", Anchor::CENTER);

    auto cpu_graph = new Graph(graph_container, 50, Colors::SEAGREEN);
    new Label(cpu_graph, "CPU", Anchor::CENTER);

    new Label(panel_container, environment().get("POSIX").get("LOGNAME").as_string());

    auto clock_timer = own<Timer>(1000, [&]() {
        TimeStamp timestamp = timestamp_now();
        DateTime datetime = timestamp_to_datetime(timestamp);

        char buffer[256];
        snprintf(buffer, 256, "%02d:%02d:%02d ", datetime.hour, datetime.minute, datetime.second);

        widget_date_and_time->text(buffer);
    });

    clock_timer->start();

    auto ram_timer = own<Timer>(500, [&]() {
        SystemStatus status{};
        hj_system_status(&status);

        ram_graph->record(status.used_ram / (float)status.total_ram);
    });

    ram_timer->start();

    auto cpu_timer = own<Timer>(100, [&]() {
        SystemStatus status{};
        hj_system_status(&status);
        cpu_graph->record(status.cpu_usage / 100.0);
    });

    cpu_timer->start();

    graph_container->on(Event::ACTION, [](auto) { process_run("task-manager", nullptr); });

    window->show();

    return Application::run();
}
