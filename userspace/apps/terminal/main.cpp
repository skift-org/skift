#include <libwidget/Application.h>
#include <libwidget/ScrollBar.h>
#include <libwidget/TitleBar.h>
#include <libwidget/Window.h>

#include "terminal/TerminalWidget.h"

int main(int argc, char **argv)
{
    Widget::Application::initialize(argc, argv);

    theme_set_color(Widget::THEME_BACKGROUND, Widget::theme_get_color(Widget::THEME_ANSI_BACKGROUND));

    Widget::Window *window = new Widget::Window(WINDOW_RESIZABLE | WINDOW_ACRYLIC);

    window->size(Math::Vec2i(700, 500));
    window->opacity(0.85);

    window->root()->layout(VFLOW(0));

    new Widget::TitleBar(
        window->root(),
        Graphic::Icon::get("console-line"),
        "Terminal");

    auto widget = new TerminalWidget(window->root());
    widget->focus();
    widget->flags(Widget::Component::FILL);
    widget->outsets({0, 6, 6, 6});

    window->show();

    return Widget::Application::run();
}
