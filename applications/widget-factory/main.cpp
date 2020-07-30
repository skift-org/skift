#include <libwidget/Application.h>
#include <libwidget/Widgets.h>

int main(int argc, char **argv)
{
    application_initialize(argc, argv);

    Window *window = window_create(WINDOW_RESIZABLE);

    window_set_icon(window, Icon::get("widgets"));
    window_set_title(window, "Widget Factory");
    window_set_size(window, Vec2i(500, 400));

    window_root(window)->layout = VFLOW(8);

    panel_create(window_root(window));
    panel_create(window_root(window));

    Widget *panel_hflow = container_create(window_root(window));
    {
        panel_hflow->layout = HFLOW(8);

        panel_create(panel_hflow)->layout_attributes = LAYOUT_FILL;
        panel_create(panel_hflow)->layout_attributes = LAYOUT_FILL;
        button_create_with_text(panel_hflow, BUTTON_TEXT, "Hello, world!")->layout_attributes = LAYOUT_FILL;
        panel_create(panel_hflow)->layout_attributes = LAYOUT_FILL;
        panel_create(panel_hflow)->layout_attributes = LAYOUT_FILL;
    }

    label_create(window_root(window), "Buttons");
    Widget *buttons = container_create(window_root(window));
    {
        buttons->layout = HFLOW(8);
        buttons->insets = Insets(0, 8);

        button_create_with_text(buttons, BUTTON_TEXT, "BUTTON");
        button_create_with_text(buttons, BUTTON_OUTLINE, "BUTTON");
        button_create_with_text(buttons, BUTTON_FILLED, "BUTTON");
        button_create_with_icon_and_text(buttons, BUTTON_TEXT, Icon::get("widgets"), "BUTTON");
        button_create_with_icon_and_text(buttons, BUTTON_OUTLINE, Icon::get("widgets"), "BUTTON");
        button_create_with_icon_and_text(buttons, BUTTON_FILLED, Icon::get("widgets"), "BUTTON");
    }

    label_create(window_root(window), "Grid layout");

    Widget *panel_grid = container_create(window_root(window));
    {
        panel_grid->layout = GRID(3, 3, 2, 4);
        panel_grid->layout_attributes = LAYOUT_FILL;

        panel_create(panel_grid);
        panel_create(panel_grid);
        panel_create(panel_grid);
        panel_create(panel_grid);

        button_create_with_text(panel_grid, BUTTON_FILLED, "Grid layout!");

        panel_create(panel_grid);
        panel_create(panel_grid);
        panel_create(panel_grid);
        panel_create(panel_grid);
    }

    panel_create(window_root(window));

    window_show(window);

    return application_run();
}
