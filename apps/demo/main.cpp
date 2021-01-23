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
    Result result = Application::initialize(argc, argv);

    if (result != SUCCESS)
    {
        return -1;
    }

    Window *window = new Window(WINDOW_RESIZABLE);
    window->icon(Icon::get("duck"));
    window->title("Demos");
    window->size(Vec2i(500, 400));

    window->root()->layout(VFLOW(0));

    new TitleBar(window->root());

    Widget *navbar = new Container(window->root());

    navbar->insets(Insetsi(4, 4));
    navbar->layout(HGRID(4));

    DemoWidget *demo_widget = new DemoWidget(window->root());
    demo_widget->flags(Widget::FILL);
    demo_widget->demo(&_demos[0]);

    for (size_t i = 0; _demos[i].name; i++)
    {
        Widget *demo_button = new Button(navbar, Button::TEXT, _demos[i].name);

        demo_button->on(Event::ACTION, [i, demo_widget](auto) {
            demo_widget->demo(&_demos[i]);
        });
    }

    window->show();

    return Application::run();
}
