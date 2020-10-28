#include <libwidget/Application.h>
#include <libwidget/Widgets.h>

int main(int argc, char **argv)
{
    application_initialize(argc, argv);

    Window *window = new Window(WINDOW_RESIZABLE);

    window->icon(Icon::get("widgets"));
    window->title("Widget Factory");
    window->size(Vec2i(500, 400));

    window->root()->layout(VFLOW(8));

    new Panel(window->root());
    new Panel(window->root());

    Widget *panel_hflow = new Container(window->root());
    {
        panel_hflow->layout(HFLOW(8));

        auto p0 = new Panel(panel_hflow);
        p0->attributes(LAYOUT_FILL);

        auto p1 = new Panel(panel_hflow);
        p1->attributes(LAYOUT_FILL);

        auto button = new Button(panel_hflow, BUTTON_TEXT, "Hello, world!");
        button->attributes(LAYOUT_FILL);

        auto p2 = new Panel(panel_hflow);
        p2->attributes(LAYOUT_FILL);

        auto p3 = new Panel(panel_hflow);
        p3->attributes(LAYOUT_FILL);
    }

    new Label(window->root(), "Buttons", Position::CENTER);
    Widget *buttons = new Container(window->root());
    {
        buttons->layout(HFLOW(8));
        buttons->insets(Insets(0, 8));

        new Button(buttons, BUTTON_TEXT, "BUTTON");
        new Button(buttons, BUTTON_OUTLINE, "BUTTON");
        new Button(buttons, BUTTON_FILLED, "BUTTON");
        new Button(buttons, BUTTON_TEXT, Icon::get("widgets"), "BUTTON");
        new Button(buttons, BUTTON_OUTLINE, Icon::get("widgets"), "BUTTON");
        new Button(buttons, BUTTON_FILLED, Icon::get("widgets"), "BUTTON");
    }

    new Label(window->root(), "Grid layout", Position::CENTER);

    Widget *panel_grid = new Container(window->root());
    {
        panel_grid->layout(GRID(3, 3, 4, 4));
        panel_grid->attributes(LAYOUT_FILL);

        new Panel(panel_grid);
        new Slider(panel_grid);
        new Panel(panel_grid);
        new Panel(panel_grid);

        new Button(panel_grid, BUTTON_FILLED, "Grid layout!");

        new Panel(panel_grid);
        new Panel(panel_grid);
        new Panel(panel_grid);
        new Panel(panel_grid);
    }

    window->show();

    return application_run();
}
