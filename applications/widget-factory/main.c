#include <libwidget/Application.h>
#include <libwidget/Button.h>
#include <libwidget/Container.h>
#include <libwidget/Panel.h>

int main(int argc, char **argv)
{
    application_initialize(argc, argv);

    Window *window = window_create("widgets", "Widget Factory", 500, 400, WINDOW_RESIZABLE);

    window_root(window)->layout = VGRID(8);

    panel_create(window_root(window));
    panel_create(window_root(window));

    Widget *panel_hflow = container_create(window_root(window));
    {
        panel_hflow->layout = HFLOW(8);

        panel_create(panel_hflow)->layout_attributes = LAYOUT_FILL;
        panel_create(panel_hflow)->layout_attributes = LAYOUT_FILL;
        panel_create(panel_hflow)->layout_attributes = LAYOUT_FILL;
        panel_create(panel_hflow)->layout_attributes = LAYOUT_FILL;
        button_create(panel_hflow, "Hello, world!")->layout_attributes = LAYOUT_FILL;
        panel_create(panel_hflow)->layout_attributes = LAYOUT_FILL;
        panel_create(panel_hflow)->layout_attributes = LAYOUT_FILL;
        panel_create(panel_hflow)->layout_attributes = LAYOUT_FILL;
        panel_create(panel_hflow)->layout_attributes = LAYOUT_FILL;
    }

    Widget *panel_hgrid = container_create(window_root(window));
    {
        panel_hgrid->layout = HGRID(8);

        panel_create(panel_hgrid);
        panel_create(panel_hgrid);
        panel_create(panel_hgrid);
        panel_create(panel_hgrid);
        button_create(panel_hgrid, "Hello, world!");
        panel_create(panel_hgrid);
        panel_create(panel_hgrid);
        panel_create(panel_hgrid);
        panel_create(panel_hgrid);
    }

    Widget *panel_grid = container_create(window_root(window));
    {
        panel_grid->layout = GRID(3, 3, 2, 4);

        panel_create(panel_grid);
        panel_create(panel_grid);
        panel_create(panel_grid);
        panel_create(panel_grid);

        button_create(panel_grid, "Grid layout!");

        panel_create(panel_grid);
        panel_create(panel_grid);
        panel_create(panel_grid);
        panel_create(panel_grid);
    }

    panel_create(window_root(window));

    window_show(window);

    return application_run();
}
