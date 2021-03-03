#include <libfilepicker/FilePicker.h>
#include <libwidget/Application.h>
#include <libwidget/Container.h>
#include <libwidget/Label.h>
#include <libwidget/Panel.h>
#include <libwidget/Switch.h>
#include <libwidget/TextField.h>
#include <libwidget/TitleBar.h>

int main(int argc, char **argv)
{
    Application::initialize(argc, argv);

    auto acrylic_window = own<Window>(WINDOW_ACRYLIC);
    acrylic_window->title("Acrylic!");
    new TitleBar(acrylic_window->root());

    auto window = own<Window>(WINDOW_RESIZABLE);

    window->icon(Icon::get("widgets"));
    window->title("Widget Factory");
    window->size(Vec2i(500, 400));

    window->root()->layout(VFLOW(8));

    new TitleBar(window->root());

    Widget *panel_hflow = new Container(window->root());
    {
        panel_hflow->layout(HFLOW(8));

        auto p1 = new Panel(panel_hflow);
        p1->flags(Widget::FILL);

        auto button = new Button(panel_hflow, Button::TEXT, "Hello, world!");
        button->flags(Widget::FILL);

        auto p2 = new Container(panel_hflow);
        p2->layout(STACK());
        p2->flags(Widget::FILL);

        auto p3 = new Panel(panel_hflow);
        p3->flags(Widget::FILL);
    }

    new Label(window->root(), "Buttons", Anchor::CENTER);
    Widget *buttons = new Container(window->root());
    {
        buttons->layout(HFLOW(8));
        buttons->insets(Insetsi(0, 8));

        new Button(buttons, Button::TEXT, "BUTTON");
        new Button(buttons, Button::OUTLINE, "BUTTON");
        new Button(buttons, Button::FILLED, "BUTTON");
        new Button(buttons, Button::TEXT, Icon::get("widgets"), "BUTTON");
        new Button(buttons, Button::OUTLINE, Icon::get("widgets"), "BUTTON");
        new Button(buttons, Button::FILLED, Icon::get("widgets"), "BUTTON");
    }

    new Label(window->root(), "Grid layout", Anchor::CENTER);

    Widget *panel_grid = new Container(window->root());
    {
        panel_grid->layout(GRID(3, 3, 4, 4));
        panel_grid->flags(Widget::FILL);

        new Panel(panel_grid);
        new TextField(panel_grid, TextModel::empty());
        auto text_field = new TextField(panel_grid, TextModel::empty());
        text_field->focus();

        auto acrylic_button = new Button(panel_grid, Button::FILLED, "Open acrylic window !");
        acrylic_button->on(Event::ACTION, [&](auto) {
            acrylic_window->show();
        });

        new Button(panel_grid, Button::FILLED, "Grid layout!");

        auto dialog_button = new Button(panel_grid, Button::FILLED, "Open dialog!");
        dialog_button->on(Event::ACTION, [&](auto) {
            filepicker::Dialog picker{};
            picker.show();
        });

        new Panel(panel_grid);
        new Panel(panel_grid);
        new Panel(panel_grid);
    }

    window->show();

    return Application::run();
}
