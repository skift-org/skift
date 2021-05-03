#include <libwidget/Application.h>
#include <libwidget/Components.h>
#include <libwidget/Window.h>

int main(int, char **)
{
    Widget::Window *window = new Widget::Window(WINDOW_RESIZABLE);
    window->size(Math::Vec2i(500, 400));

    window->root()->add(Widget::titlebar(Graphic::Icon::get("font-format"), "Font manager"));

    auto navbar = window->root()->add<Widget::Element>();

    navbar->insets(Insetsi(4, 4));

    window->show();

    return Widget::Application::the().run();
}
