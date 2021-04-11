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
    Widget::Application::initialize(argc, argv);

    auto acrylic_window = own<Widget::Window>(WINDOW_ACRYLIC);
    acrylic_window->title("Acrylic!");
    new Widget::TitleBar(acrylic_window->root());

    auto window = own<Widget::Window>(WINDOW_RESIZABLE);

    window->icon(Graphic::Icon::get("widgets"));
    window->title("Widget Factory");
    window->size(Math::Vec2i(500, 400));

    window->root()->layout(VFLOW(8));

    new Widget::TitleBar(window->root());

    Widget::Component *panel_hflow = new Widget::Container(window->root());
    {
        panel_hflow->layout(HFLOW(8));

        auto p1 = new Widget::Panel(panel_hflow);
        p1->flags(Widget::Component::FILL);

        auto button = new Widget::Button(panel_hflow, Widget::Button::TEXT, "Hello, world!");
        button->flags(Widget::Component::FILL);

        auto p2 = new Widget::Container(panel_hflow);
        p2->layout(STACK());
        p2->flags(Widget::Component::FILL);

        auto p3 = new Widget::Panel(panel_hflow);
        p3->flags(Widget::Component::FILL);
    }

    new Widget::Label(window->root(), "Buttons", Anchor::CENTER);
    Widget::Component *buttons = new Widget::Container(window->root());
    {
        buttons->layout(HFLOW(8));
        buttons->insets(Insetsi(0, 8));

        new Widget::Button(buttons, Widget::Button::TEXT, "BUTTON");
        new Widget::Button(buttons, Widget::Button::OUTLINE, "BUTTON");
        new Widget::Button(buttons, Widget::Button::FILLED, "BUTTON");
        new Widget::Button(buttons, Widget::Button::TEXT, Graphic::Icon::get("widgets"), "BUTTON");
        new Widget::Button(buttons, Widget::Button::OUTLINE, Graphic::Icon::get("widgets"), "BUTTON");
        new Widget::Button(buttons, Widget::Button::FILLED, Graphic::Icon::get("widgets"), "BUTTON");
    }

    new Widget::Label(window->root(), "Grid layout", Anchor::CENTER);

    Widget::Component *panel_grid = new Widget::Container(window->root());
    {
        panel_grid->layout(GRID(3, 3, 4, 4));
        panel_grid->flags(Widget::Component::FILL);

        new Widget::Panel(panel_grid);
        new Widget::TextField(panel_grid, Widget::TextModel::empty());
        auto text_field = new Widget::TextField(panel_grid, Widget::TextModel::empty());
        text_field->focus();

        auto acrylic_button = new Widget::Button(panel_grid, Widget::Button::FILLED, "Open acrylic window !");
        acrylic_button->on(Widget::Event::ACTION, [&](auto) {
            acrylic_window->show();
        });

        new Widget::Button(panel_grid, Widget::Button::FILLED, "Grid layout!");

        auto dialog_button = new Widget::Button(panel_grid, Widget::Button::FILLED, "Open dialog!");
        dialog_button->on(Widget::Event::ACTION, [&](auto) {
            FilePicker::Dialog picker{};
            picker.show();
        });

        new Widget::Panel(panel_grid);
        new Widget::Panel(panel_grid);
        new Widget::Panel(panel_grid);
    }

    window->show();

    return Widget::Application::run();
}
