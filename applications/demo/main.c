#include "demo/DemoWidget.h"

static int _current_demo = 0;

static Demo _demos[] = {
    {"Colors", colors_draw},
    {"Cube", cube_draw},
    {"Graphics", graphics_draw},
    {"Lines", lines_draw},
};

int main(int argc, char **argv)
{
    Result result = application_initialize(argc, argv);

    if (result != SUCCESS)
    {
        return -1;
    }

    Window *main_window = window_create("Demos", 500, 400);

    Widget *demo_widget = demo_widget_create(window_root(main_window));

    demo_widget_set_demo(demo_widget, &_demos[_current_demo]);

    return application_run();
}
