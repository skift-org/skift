#include <libwidget/Widgets.h>

#include "demo/DemoWidget.h"

static Demo _demos[] = {
    {"Latency", latency_draw},
    {"Colors", colors_draw},
    {"Cube", cube_draw},
    {"Graphics", graphics_draw},
    {"Lines", lines_draw},
    {NULL, NULL},
};

Widget *_demo_widget = NULL;

void set_current_demo_callback(Demo *demo, Widget *sender, Event *event)
{
    __unused(sender);
    __unused(event);

    demo_widget_set_demo(_demo_widget, demo);
}

int main(int argc, char **argv)
{
    Result result = application_initialize(argc, argv);

    if (result != SUCCESS)
    {
        return -1;
    }

    Window *window = window_create(WINDOW_RESIZABLE);

    window_set_icon(window, icon_get("duck"));
    window_set_title(window, "Demos");
    window_set_size(window, vec2i(500, 400));

    window_root(window)->layout = VFLOW(0);

    Widget *side_bar = container_create(window_root(window));
    side_bar->insets = INSETS(4, 4);
    side_bar->layout = HGRID(4);
    side_bar->bound.height = 32;

    for (size_t i = 0; _demos[i].name; i++)
    {
        Widget *demo_button = button_create_with_text(side_bar, BUTTON_TEXT, _demos[i].name);
        widget_set_event_handler(demo_button, EVENT_ACTION, EVENT_HANDLER(&_demos[i], (EventHandlerCallback)set_current_demo_callback));
    }

    side_bar->bound.width = 128;

    _demo_widget = demo_widget_create(window_root(window));
    _demo_widget->layout_attributes = LAYOUT_FILL;

    demo_widget_set_demo(_demo_widget, &_demos[0]);

    window_show(window);

    return application_run();
}
