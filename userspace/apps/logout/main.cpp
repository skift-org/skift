#include <abi/Syscalls.h>

#include <libwidget/Application.h>
#include <libwidget/Button.h>
#include <libwidget/Container.h>
#include <libwidget/IconPanel.h>
#include <libwidget/Label.h>
#include <libwidget/Panel.h>
#include <libwidget/Screen.h>
#include <libwidget/Spacer.h>

int main(int argc, char **argv)
{
    Widget::Application::initialize(argc, argv);

    Widget::Window *window = new Widget::Window(WINDOW_BORDERLESS | WINDOW_ALWAYS_FOCUSED | WINDOW_ACRYLIC | WINDOW_NO_ROUNDED_CORNERS);

    window->type(WINDOW_TYPE_POPOVER);
    window->bound(Widget::Screen::bound());
    window->opacity(0);
    window->root()->layout(STACK());

    auto background = new Widget::Panel(window->root());

    background->layout(STACK());
    background->color(Widget::THEME_MIDDLEGROUND, Graphic::Colors::BLACK.with_alpha(0.5));
    background->flags(Widget::Component::FILL);

    auto dialog = new Widget::Container(background);

    dialog->min_width(256);
    dialog->min_height(256);
    dialog->layout(VFLOW(8));

    auto icon_and_title_container = new Widget::Panel(dialog);
    icon_and_title_container->layout(HFLOW(4));
    icon_and_title_container->border_radius(6);
    icon_and_title_container->insets(Insetsi{8});

    auto title_icon = new Widget::IconPanel(icon_and_title_container, Graphic::Icon::get("power-standby"));
    title_icon->icon_size(Graphic::ICON_36PX);

    auto warning_container = new Widget::Container(icon_and_title_container);
    warning_container->flags(Widget::Component::FILL);
    warning_container->layout(VGRID(2));

    new Widget::Label(warning_container, "Shutdown or restart your computer.", Anchor::BOTTOM_LEFT);
    new Widget::Label(warning_container, "Any unsaved work will be lost!", Anchor::TOP_LEFT);

    new Widget::Spacer(dialog);

    auto shutdown_button = new Widget::Button(dialog, Widget::Button::TEXT, Graphic::Icon::get("power-standby"), "Shutdown");

    shutdown_button->on(Widget::EventType::ACTION, [&](auto) {
        hj_system_shutdown();
    });

    auto reboot_button = new Widget::Button(dialog, Widget::Button::TEXT, Graphic::Icon::get("restart"), "Reboot");

    reboot_button->on(Widget::EventType::ACTION, [&](auto) {
        hj_system_reboot();
    });

    new Widget::Button(dialog, Widget::Button::TEXT, Graphic::Icon::get("logout"), "Logoff");

    new Widget::Spacer(dialog);

    auto cancel_button = new Widget::Button(dialog, Widget::Button::FILLED, "Cancel");

    cancel_button->on(Widget::EventType::ACTION, [&](auto) {
        window->hide();
    });

    window->on(Widget::Event::KEYBOARD_KEY_PRESS, [&](Widget::Event *event) {
        if (event->keyboard.key == KEYBOARD_KEY_ESC)
        {
            Widget::Application::exit(PROCESS_SUCCESS);
        }
    });

    window->show();

    return Widget::Application::run();
}
