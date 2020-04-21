#include <libwidget/Application.h>
#include <libwidget/Button.h>
#include <libwidget/Container.h>
#include <libwidget/Panel.h>

int main(int argc, char **argv)
{
    application_initialize(argc, argv);

    Window *window = window_create("widgets", "Widget Factory", 500, 400, WINDOW_NONE);

    window_root(window)->layout = (Layout){LAYOUT_VGRID, 0, 8};

    panel_create(window_root(window));
    panel_create(window_root(window));

    Widget *panel_hflow = container_create(window_root(window));
    {
        panel_hflow->layout = (Layout){LAYOUT_HFLOW, 8, 0};

        panel_create(panel_hflow);
        button_create(panel_hflow, "Hello, world!")->layout_attributes = LAYOUT_FILL;
        button_create(panel_hflow, "Hello, world!")->layout_attributes = LAYOUT_FILL;
        button_create(panel_hflow, "Hello, world!")->layout_attributes = LAYOUT_FILL;
        panel_create(panel_hflow);
    }

    Widget *panel_hgrid = container_create(window_root(window));
    {
        panel_hgrid->layout = (Layout){LAYOUT_HGRID, 8, 0};

        panel_create(panel_hgrid);
        panel_create(panel_hgrid);
        panel_create(panel_hgrid);

        button_create(panel_hgrid, "Hello, world!");
    }

    panel_create(window_root(window));

    window_show(window);

    return application_run();
}
