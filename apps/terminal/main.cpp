#include <libwidget/Application.h>
#include <libwidget/Window.h>
#include <libwidget/widgets/ScrollBar.h>

#include "terminal/TerminalWidget.h"

int main(int argc, char **argv)
{
    Application::initialize(argc, argv);

    theme_set_color(THEME_BACKGROUND, theme_get_color(THEME_ANSI_BACKGROUND));

    Window *window = new Window(WINDOW_RESIZABLE);

    window->icon(Icon::get("console-line"));
    window->title("Terminal");
    window->size(Vec2i(700, 500));

    window->root()->insets(Insetsi{0, 6, 6, 6});
    window->root()->layout(HFLOW(0));

    auto widget = new TerminalWidget(window->root());
    widget->focus();
    widget->flags(Widget::FILL);

    window->show();

    return Application::run();
}
