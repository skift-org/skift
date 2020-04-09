#include <libwidget/Application.h>
#include <libwidget/Button.h>
#include <libwidget/Container.h>
#include <libwidget/Panel.h>

int main(int argc, char **argv)
{
    application_initialize(argc, argv);

    Window *window = window_create("Widget Factory", 500, 400);

    window_root(window)->layout = (Layout){LAYOUT_VFLOW, 0, 8};

    panel_create(window_root(window));
    panel_create(window_root(window));

    Widget *panel = container_create(window_root(window));
    {
        panel->layout = (Layout){LAYOUT_HFLOW, 8, 0};

        panel_create(panel);
        panel_create(panel);
        panel_create(panel);

        button_create(panel, "Hello, world!");
    }

    panel_create(window_root(window));

    return application_run();
}
