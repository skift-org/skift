#include <libwidget/Components.h>

#include "demo/widgets/DemoWidget.h"

static Demo _demos[] = {
    {"Path", path_draw},
    {"Colors", colors_draw},
    {"Graphics", graphics_draw},
    {"Lines", lines_draw},
    {nullptr, nullptr},
};

int main(int, char **)
{
    Widget::Window *window = new Widget::Window(WINDOW_RESIZABLE);
    window->size(Math::Vec2i(500, 400));

    window->root()->DONT_USE_ME_layout(VFLOW(0));

    window->root()->add(Widget::titlebar(Graphic::Icon::get("duck"), "Demos"));

    auto navbar = window->root()->add<Widget::Element>();

    navbar->insets(Insetsi(4, 4));
    navbar->DONT_USE_ME_layout(VFLOW(4));

    auto demo_widget = window->root()->add<DemoWidget>();
    demo_widget->flags(Widget::Element::FILL);
    demo_widget->demo(&_demos[0]);

    for (size_t i = 0; _demos[i].name; i++)
    {
        auto demo_button = navbar->add(Widget::basic_button(_demos[i].name, [i, demo_widget] {
            demo_widget->demo(&_demos[i]);
        }));
    }

    window->show();

    return Widget::Application::the()->run();
}
