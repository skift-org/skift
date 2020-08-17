#include <libwidget/Widgets.h>

#include "demo/DemoWidget.h"

static Demo _demos[] = {
    {"Fonts", fonts_draw},
    {"Latency", latency_draw},
    {"Colors", colors_draw},
    {"Graphics", graphics_draw},
    {"Lines", lines_draw},
    {nullptr, nullptr},
};

int main(int argc, char **argv)
{
    Result result = application_initialize(argc, argv);

    if (result != SUCCESS)
    {
        return -1;
    }

    Window *window = window_create(WINDOW_RESIZABLE);
    window->icon(Icon::get("duck"));
    window->title("Demos");
    window->size(Vec2i(500, 400));

    window_root(window)->layout(VFLOW(0));

    Widget *navbar = new Container(window_root(window));
    navbar->insets(Insets(4, 4));
    navbar->layout(HGRID(4));
    navbar->bound(navbar->bound().with_height(32));
    navbar->bound(navbar->bound().with_width(128));

    DemoWidget *demo_widget = nullptr;
    demo_widget = new DemoWidget(window_root(window));
    demo_widget->attributes(LAYOUT_FILL);
    demo_widget->demo(&_demos[0]);

    for (size_t i = 0; _demos[i].name; i++)
    {
        Widget *demo_button = new Button(navbar, BUTTON_TEXT, _demos[i].name);

        demo_button->on(Event::ACTION, [i, demo_widget](auto) {
            demo_widget->demo(&_demos[i]);
        });
    }

    window_show(window);

    return application_run();
}
