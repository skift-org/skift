#include <libfilepicker/FilePicker.h>
#include <libwidget/Application.h>
#include <libwidget/Components.h>
#include <libwidget/Layouts.h>

int main(int, char **)
{
    auto acrylic_window = own<Widget::Window>(WINDOW_ACRYLIC);
    acrylic_window->root()->add(Widget::titlebar(Graphic::Icon::get("widgets"), "Acrylic!"));

    auto window = own<Widget::Window>(WINDOW_RESIZABLE);

    window->size(Math::Vec2i(500, 400));

    window->root()->add(Widget::titlebar(Graphic::Icon::get("widgets"), "Widget Factory"));

    auto panel_hflow = window->root()->add<Widget::Element>();
    {
        auto p1 = panel_hflow->add(Widget::panel());
        p1->flags(Widget::Element::FILL);

        auto button = panel_hflow->add(Widget::basic_button("Hello, world!"));
        button->flags(Widget::Element::FILL);

        auto p2 = panel_hflow->add<Widget::Element>();
        p2->flags(Widget::Element::FILL);

        auto p3 = panel_hflow->add(Widget::panel());
        p3->flags(Widget::Element::FILL);
    }

    window->root()->add(Widget::label("Buttons", Math::Anchor::CENTER));
    auto buttons = window->root()->add<Widget::Element>();
    {
        buttons->add(Widget::basic_button("BUTTON"));
        buttons->add(Widget::outline_button("BUTTON"));
        buttons->add(Widget::filled_button("BUTTON"));
        buttons->add(Widget::basic_button(Graphic::Icon::get("widgets"), "BUTTON"));
        buttons->add(Widget::outline_button(Graphic::Icon::get("widgets"), "BUTTON"));
        buttons->add(Widget::filled_button(Graphic::Icon::get("widgets"), "BUTTON"));
    }

    window->root()->add(Widget::label("Grid layout", Math::Anchor::CENTER));

    auto panel_grid = window->root()->add(Widget::fill(Widget::grid(3, 3, 4, 4, {})));
    {
        panel_grid->add(Widget::panel());
        panel_grid->add<Widget::TextField>(Widget::TextModel::empty());
        auto text_field = panel_grid->add<Widget::TextField>(Widget::TextModel::empty());
        text_field->focus();

        auto acrylic_button = panel_grid->add(Widget::filled_button("Open acrylic window !", [&] {
            acrylic_window->show();
        }));

        panel_grid->add(Widget::filled_button("Grid layout!"));

        auto dialog_button = panel_grid->add(Widget::filled_button("Open dialog!", [&] {
            FilePicker::Dialog picker{};
            picker.show();
        }));

        panel_grid->add(Widget::panel());
        panel_grid->add(Widget::panel());
        panel_grid->add(Widget::panel());
    }

    window->show();

    return Widget::Application::the().run();
}
