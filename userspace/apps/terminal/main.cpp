#include <libwidget/Application.h>
#include <libwidget/Components.h>
#include <libwidget/Window.h>

#include "terminal/TerminalWidget.h"

int main(int, char **)
{
    theme_set_color(Widget::THEME_BACKGROUND, Widget::theme_get_color(Widget::THEME_ANSI_BACKGROUND));

    Widget::Window *window = new Widget::Window(WINDOW_RESIZABLE | WINDOW_ACRYLIC);

    window->size(Math::Vec2i(700, 500));
    window->opacity(0.85);

    window->root()->DONT_USE_ME_layout(VFLOW(0));

    window->root()->add(Widget::titlebar(Graphic::Icon::get("console-line"), "Terminal"));

    auto widget = window->root()->add<TerminalWidget>();
    widget->focus();
    widget->flags(Widget::Element::FILL);
    widget->outsets({0, 6, 6, 6});

    window->show();

    return Widget::Application::the().run();
}
