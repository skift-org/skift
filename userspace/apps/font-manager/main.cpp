#include <libwidget/Application.h>
#include <libwidget/Container.h>
#include <libwidget/TitleBar.h>
#include <libwidget/Window.h>

int main(int, char **)
{
    Widget::Window *window = new Widget::Window(WINDOW_RESIZABLE);
    window->size(Math::Vec2i(500, 400));

    window->root()->layout(VFLOW(0));

    window->root()->add<Widget::TitleBar>(Graphic::Icon::get("duck"), "Demos");

    auto navbar = window->root()->add<Widget::Container>();

    navbar->insets(Insetsi(4, 4));
    navbar->layout(HGRID(4));

    window->show();

    return Widget::Application::the()->run();
}
