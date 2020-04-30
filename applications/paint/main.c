#include <libwidget/Application.h>
#include <libwidget/Icon.h>
#include <libwidget/Panel.h>
#include <libwidget/Separator.h>

#include "paint/Canvas.h"

int main(int argc, char **argv)
{
    application_initialize(argc, argv);

    Window *window = window_create("brush", "Paint", 500, 400, WINDOW_RESIZABLE);

    Widget *root = window_root((Window *)window);
    root->layout = (Layout){LAYOUT_VFLOW, 0, 0};

    Widget *toolbar = panel_create(window_root(window));

    toolbar->layout = (Layout){LAYOUT_HFLOW, 12, 0};
    toolbar->insets = INSETS(8, 8);

    icon_create(toolbar, "pencil");
    icon_create(toolbar, "brush");
    icon_create(toolbar, "eraser");
    separator_create(toolbar);
    icon_create(toolbar, "format-text-variant");
    icon_create(toolbar, "vector-line");
    icon_create(toolbar, "rectangle-outline");
    icon_create(toolbar, "circle-outline");
    separator_create(toolbar);
    icon_create(toolbar, "format-color-fill");
    icon_create(toolbar, "eyedropper");

    Widget *canvas = canvas_create(window_root(window), 300, 300);
    canvas->layout_attributes = LAYOUT_FILL;

    window_show(window);

    return application_run();
}
