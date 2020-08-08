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

Widget *_demo_widget = nullptr;

int main(int argc, char **argv)
{
    Result result = application_initialize(argc, argv);

    if (result != SUCCESS)
    {
        return -1;
    }

    Window *window = window_create(WINDOW_RESIZABLE);

    window_set_icon(window, Icon::get("duck"));
    window_set_title(window, "Demos");
    window_set_size(window, Vec2i(500, 400));

    window_root(window)->layout = VFLOW(0);

    Widget *navbar = container_create(window_root(window));
    navbar->insets = Insets(4, 4);
    navbar->layout = HGRID(4);
    navbar->bound = navbar->bound.with_width(32);

    for (size_t i = 0; _demos[i].name; i++)
    {
        Widget *demo_button = button_create_with_text(navbar, BUTTON_TEXT, _demos[i].name);

        demo_button->on(Event::ACTION, [i](auto) {
            demo_widget_set_demo(_demo_widget, &_demos[i]);
        });
    }

    navbar->bound = navbar->bound.with_width(128);

    _demo_widget = demo_widget_create(window_root(window));
    _demo_widget->layout_attributes = LAYOUT_FILL;

    demo_widget_set_demo(_demo_widget, &_demos[0]);

    window_show(window);

    return application_run();
}
