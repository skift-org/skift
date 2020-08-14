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

    new Panel(window_root(window));
    new Panel(window_root(window));

    Widget *panel_hflow = new Container(window_root(window));
    {
        panel_hflow->layout = HFLOW(8);

        auto p0 = new Panel(panel_hflow);
        p0->layout_attributes = LAYOUT_FILL;

        auto p1 = new Panel(panel_hflow);
        p1->layout_attributes = LAYOUT_FILL;

        auto button = new Button(panel_hflow, BUTTON_TEXT, "Hello, world!");
        button->layout_attributes = LAYOUT_FILL;

        auto p2 = new Panel(panel_hflow);
        p2->layout_attributes = LAYOUT_FILL;

        auto p3 = new Panel(panel_hflow);
        p3->layout_attributes = LAYOUT_FILL;
    }

    new Label(window_root(window), "Buttons");
    Widget *buttons = new Container(window_root(window));
    {
        buttons->layout = HFLOW(8);
        buttons->insets = Insets(0, 8);

        new Button(buttons, BUTTON_TEXT, "BUTTON");
        new Button(buttons, BUTTON_OUTLINE, "BUTTON");
        new Button(buttons, BUTTON_FILLED, "BUTTON");
        new Button(buttons, BUTTON_TEXT, Icon::get("widgets"), "BUTTON");
        new Button(buttons, BUTTON_OUTLINE, Icon::get("widgets"), "BUTTON");
        new Button(buttons, BUTTON_FILLED, Icon::get("widgets"), "BUTTON");
    }

    new Label(window_root(window), "Grid layout");

    Widget *panel_grid = new Container(window_root(window));
    {
        panel_grid->layout = GRID(3, 3, 2, 4);
        panel_grid->layout_attributes = LAYOUT_FILL;

        new Panel(panel_grid);
        new Panel(panel_grid);
        new Panel(panel_grid);
        new Panel(panel_grid);

        new Button(panel_grid, BUTTON_FILLED, "Grid layout!");

        new Panel(panel_grid);
        new Panel(panel_grid);
        new Panel(panel_grid);
        new Panel(panel_grid);
    }

    new Panel(window_root(window));

    window_show(window);

    return application_run();
}
