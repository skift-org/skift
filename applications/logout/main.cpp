#include <abi/Syscalls.h>
#include <libwidget/Application.h>
#include <libwidget/Screen.h>
#include <libwidget/Widgets.h>

int main(int argc, char **argv)
{
    Application::initialize(argc, argv);

    Window *window = new Window(WINDOW_BORDERLESS | WINDOW_ALWAYS_FOCUSED | WINDOW_TRANSPARENT);

    window->type(WINDOW_TYPE_PANEL);
    window->bound(Screen::bound());
    window->opacity(0);
    window->show();
    window->root()->layout(STACK());

    auto background = new Panel(window->root());

    background->layout(STACK());
    background->color(THEME_MIDDLEGROUND, Colors::BLACK.with_alpha(0.5));

    auto dialog = new Container(background);

    dialog->max_width(256);
    dialog->max_height(256);
    dialog->layout(VFLOW(8));

    auto icon_and_title_container = new Panel(dialog);
    icon_and_title_container->layout(HFLOW(4));
    icon_and_title_container->border_radius(6);
    icon_and_title_container->insets(Insetsi{8});

    auto title_icon = new IconPanel(icon_and_title_container, Icon::get("power-standby"));
    title_icon->icon_size(ICON_36PX);

    auto warning_container = new Container(icon_and_title_container);
    warning_container->flags(Widget::FILL);
    warning_container->layout(VGRID(6));

    new Label(warning_container, "Shutdown or restart your computer.", Anchor::LEFT);
    new Label(warning_container, "Any unsaved work will be lost!", Anchor::LEFT);

    (new Container(dialog))->flags(Widget::FILL);

    auto shutdown_button = new Button(dialog, BUTTON_TEXT, Icon::get("power-standby"), "Shutdown");

    shutdown_button->on(EventType::ACTION, [&](auto) {
        hj_system_shutdown();
    });

    auto reboot_button = new Button(dialog, BUTTON_TEXT, Icon::get("restart"), "Reboot");

    reboot_button->on(EventType::ACTION, [&](auto) {
        hj_system_reboot();
    });

    new Button(dialog, BUTTON_TEXT, Icon::get("logout"), "Logoff");

    (new Container(dialog))->flags(Widget::FILL);

    auto cancel_button = new Button(dialog, BUTTON_FILLED, "Cancel");

    cancel_button->on(EventType::ACTION, [&](auto) {
        window->hide();
    });

    return Application::run();
}
