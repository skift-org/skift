#include <libwidget/Application.h>
#include <libwidget/ScrollBar.h>
#include <libwidget/TitleBar.h>
#include <libwidget/Window.h>

#include "terminal/TerminalWidget.h"

int main(int argc, char **argv)
{
    Application::initialize(argc, argv);

    theme_set_color(THEME_BACKGROUND, theme_get_color(THEME_ANSI_BACKGROUND));

    Window *window = new Window(WINDOW_RESIZABLE | WINDOW_ACRYLIC);

    window->icon(Icon::get("console-line"));
    window->title("Terminal");
    window->size(Vec2i(700, 500));
    window->opacity(0.85);

    window->root()->layout(VFLOW(0));

    new TitleBar(window->root());

    auto widget = new TerminalWidget(window->root());
    widget->focus();
    widget->flags(Widget::FILL);
    widget->outsets({0, 6, 6, 6});

    window->show();

    return Application::run();
}
