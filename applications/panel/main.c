#include <libwidget/Application.h>
#include <libwidget/Container.h>
#include <libwidget/Label.h>

int main(int argc, char **argv)
{
    application_initialize(argc, argv);

    Window *window = window_create("Panel", 800, 32);

    window_set_border_style(window, WINDOW_BORDER_NONE);

    window_root(window)->layout = (Layout){LAYOUT_HGRID, 8, 0};

    label_create(window_root(window), "skiftOS");
    label_create(window_root(window), "skiftOS");
    label_create(window_root(window), "skiftOS");

    return application_run();
}
