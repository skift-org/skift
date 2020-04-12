#include <libwidget/Button.h>
#include <libwidget/Container.h>

#include "demo/DemoWidget.h"

static int _current_demo = 0;

static Demo _demos[] = {
    {"Colors", colors_draw},
    {"Cube", cube_draw},
    {"Graphics", graphics_draw},
    {"Lines", lines_draw},
    {NULL, NULL}};

int main(int argc, char **argv)
{
    Result result = application_initialize(argc, argv);

    if (result != SUCCESS)
    {
        return -1;
    }

    Window *main_window = window_create("Demos", 500, 400);

    window_root(main_window)->layout = (Layout){LAYOUT_VFLOW, 0, 0};

    Widget *side_bar = container_create(window_root(main_window));
    side_bar->insets = INSETS(4, 0);
    side_bar->layout = (Layout){LAYOUT_HGRID, 4, 0};

    for (size_t i = 0; _demos[i].name; i++)
    {
        button_create(side_bar, _demos[i].name);
    }

    side_bar->bound.width = 128;

    Widget *demo_widget = demo_widget_create(window_root(main_window));
    demo_widget->layout_attributes = LAYOUT_FILL;

    demo_widget_set_demo(demo_widget, &_demos[_current_demo]);

    return application_run();
}
