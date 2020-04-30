#include <libwidget/Application.h>
#include <libwidget/Panel.h>

#include "paint/Canvas.h"

int main(int argc, char **argv)
{
    application_initialize(argc, argv);

    Window *window = window_create("paint", "Paint", 500, 400, WINDOW_RESIZABLE);

    window_root(window)->layout = (Layout){LAYOUT_VFLOW, 0, 8};

    Widget *toolbar = panel_create(window_root(window));

    toolbar->layout = (Layout){LAYOUT_HFLOW, 8, 0};
    toolbar->insets = INSETS(0, 8);

    Widget *canvas = canvas_create(window_root(window), 300, 300);
    canvas->layout_attributes = LAYOUT_FILL;

    window_show(window);

    return application_run();
}
