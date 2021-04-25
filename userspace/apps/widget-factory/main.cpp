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
    acrylic_window->root()->layout(VFLOW(0));
    acrylic_window->root()->add<Widget::TitleBar>(Graphic::Icon::get("widgets"), "Acrylic!");

    auto window = own<Widget::Window>(WINDOW_RESIZABLE);

    window->size(Math::Vec2i(500, 400));
    window->root()->layout(VFLOW(8));

    window->root()->add<Widget::TitleBar>(
        Graphic::Icon::get("widgets"),
        "Widget Factory");

    auto panel_hflow = window->root()->add<Widget::Container>();
    {
        panel_hflow->layout(HFLOW(8));

        auto p1 = panel_hflow->add<Widget::Panel>();
        p1->flags(Widget::Element::FILL);

        auto button = panel_hflow->add<Widget::Button>(Widget::Button::TEXT, "Hello, world!");
        button->flags(Widget::Element::FILL);

        auto p2 = panel_hflow->add<Widget::Container>();
        p2->layout(STACK());
        p2->flags(Widget::Element::FILL);

        auto p3 = panel_hflow->add<Widget::Panel>();
        p3->flags(Widget::Element::FILL);
    }

    window->root()->add<Widget::Label>("Buttons", Anchor::CENTER);
    auto buttons = window->root()->add<Widget::Container>();
    {
        buttons->layout(HFLOW(8));
        buttons->insets(Insetsi(0, 8));

        buttons->add<Widget::Button>(Widget::Button::TEXT, "BUTTON");
        buttons->add<Widget::Button>(Widget::Button::OUTLINE, "BUTTON");
        buttons->add<Widget::Button>(Widget::Button::FILLED, "BUTTON");
        buttons->add<Widget::Button>(Widget::Button::TEXT, Graphic::Icon::get("widgets"), "BUTTON");
        buttons->add<Widget::Button>(Widget::Button::OUTLINE, Graphic::Icon::get("widgets"), "BUTTON");
        buttons->add<Widget::Button>(Widget::Button::FILLED, Graphic::Icon::get("widgets"), "BUTTON");
    }

    window->root()->add<Widget::Label>("Grid layout", Anchor::CENTER);

    auto panel_grid = window->root()->add<Widget::Container>();
    {
        panel_grid->layout(GRID(3, 3, 4, 4));
        panel_grid->flags(Widget::Element::FILL);

        panel_grid->add<Widget::Panel>();
        panel_grid->add<Widget::TextField>(Widget::TextModel::empty());
        auto text_field = panel_grid->add<Widget::TextField>(Widget::TextModel::empty());
        text_field->focus();

        auto acrylic_button = panel_grid->add<Widget::Button>(Widget::Button::FILLED, "Open acrylic window !");
        acrylic_button->on(Widget::Event::ACTION, [&](auto) {
            acrylic_window->show();
        });

        panel_grid->add<Widget::Button>(Widget::Button::FILLED, "Grid layout!");

        auto dialog_button = panel_grid->add<Widget::Button>(Widget::Button::FILLED, "Open dialog!");
        dialog_button->on(Widget::Event::ACTION, [&](auto) {
            FilePicker::Dialog picker{};
            picker.show();
        });

        panel_grid->add<Widget::Panel>();
        panel_grid->add<Widget::Panel>();
        panel_grid->add<Widget::Panel>();
    }

    window->show();

    return Widget::Application::run();
}
