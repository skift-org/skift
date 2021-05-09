#include "libwidget/layouts/Flags.h"
#include <abi/Syscalls.h>

#include <libwidget/Application.h>
#include <libwidget/Elements.h>
#include <libwidget/Screen.h>

int main(int, char **)
{
    Widget::Window *window = new Widget::Window(
        WINDOW_BORDERLESS |
        WINDOW_ALWAYS_FOCUSED |
        WINDOW_ACRYLIC |
        WINDOW_NO_ROUNDED_CORNERS);

    window->type(WINDOW_TYPE_POPOVER);
    window->bound(Widget::Screen::bound());
    window->opacity(0);

    auto background = window->root()->add(Widget::panel());

    background->color(Widget::THEME_MIDDLEGROUND, Graphic::Colors::BLACK.with_alpha(0.5));
    background->flags(Widget::Element::FILL);

    auto dialog = background->add<Widget::Element>();

    dialog->min_width(256);
    dialog->min_height(256);

    auto icon_and_title_container = dialog->add(Widget::panel(6));

    icon_and_title_container->add(Widget::square(Widget::icon("power-standby", Graphic::ICON_36PX)));

    auto warning_container = icon_and_title_container->add<Widget::Element>();
    warning_container->flags(Widget::Element::FILL);

    warning_container->add(Widget::label("Shutdown or restart your computer.", Anchor::BOTTOM_LEFT));
    warning_container->add(Widget::label("Any unsaved work will be lost!", Anchor::TOP_LEFT));

    dialog->add(Widget::spacer());

    auto shutdown_button = dialog->add(Widget::basic_button(Graphic::Icon::get("power-standby"), "Shutdown", [&] {
        hj_system_shutdown();
    }));

    auto reboot_button = dialog->add(Widget::basic_button(Graphic::Icon::get("restart"), "Reboot", [&] {
        hj_system_reboot();
    }));

    dialog->add(Widget::basic_button(Graphic::Icon::get("logout"), "Logoff"));

    dialog->add(Widget::spacer());

    auto cancel_button = dialog->add(Widget::filled_button("Cancel", [&] {
        window->hide();
    }));

    window->on(Widget::Event::KEYBOARD_KEY_PRESS, [&](Widget::Event *event) {
        if (event->keyboard.key == KEYBOARD_KEY_ESC)
        {
            Widget::Application::the().exit(PROCESS_SUCCESS);
        }
    });

    window->show();

    return Widget::Application::the().run();
}
