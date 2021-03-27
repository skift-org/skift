#include <libwidget/Button.h>
#include <libwidget/Container.h>
#include <libwidget/TitleBar.h>

#include "demo/DemoWidget.h"

static Demo _demos[] = {
    {"Path", path_draw},
    {"Colors", colors_draw},
    {"Graphics", graphics_draw},
    {"Lines", lines_draw},
    {nullptr, nullptr},
};

int main(int argc, char **argv)
{
    Result result = Widget::Application::initialize(argc, argv);

    if (result != SUCCESS)
    {
        return -1;
    }

    Widget::Window *window = new Widget::Window(WINDOW_RESIZABLE);
    window->icon(Graphic::Icon::get("duck"));
    window->title("Demos");
    window->size(Vec2i(500, 400));

    window->root()->layout(VFLOW(0));

    new Widget::TitleBar(window->root());

    Widget::Component *navbar = new Widget::Container(window->root());

    navbar->insets(Insetsi(4, 4));
    navbar->layout(HGRID(4));

    DemoWidget *demo_widget = new DemoWidget(window->root());
    demo_widget->flags(Widget::Component::FILL);
    demo_widget->demo(&_demos[0]);

    for (size_t i = 0; _demos[i].name; i++)
    {
        Widget::Component *demo_button = new Widget::Button(navbar, Widget::Button::TEXT, _demos[i].name);

        demo_button->on(Widget::Event::ACTION, [i, demo_widget](auto) {
            demo_widget->demo(&_demos[i]);
        });
    }

    window->show();

    return Widget::Application::run();
}
