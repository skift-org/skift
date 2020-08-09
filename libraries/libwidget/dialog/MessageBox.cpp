#include <libwidget/Application.h>
#include <libwidget/Widgets.h>
#include <libwidget/Window.h>
#include <libwidget/dialog/Dialog.h>

DialogButton dialog_message(
    RefPtr<Icon> icon,
    const char *title,
    const char *message,
    DialogButton buttons)
{
    __unused(buttons);

    Window *window = window_create(WINDOW_NONE);

    window_set_icon(window, icon);
    window_set_title(window, title);
    window_set_size(window, Vec2i(300, 200));
    window_root(window)->layout = VFLOW(0);
    window_root(window)->insets = Insets(8);

    window->on(Event::WINDOW_CLOSING, [window](auto) {
        application_exit_nested(DIALOG_BUTTON_CLOSED);
        window_hide(window);
    });

    Widget *message_label = label_create(window_root(window), message);
    message_label->layout_attributes = LAYOUT_FILL;

    Widget *buttons_container = container_create(window_root(window));
    buttons_container->layout = HFLOW(4);
    container_create(buttons_container)->layout_attributes = LAYOUT_FILL;

    Widget *button_no = button_create_with_text(buttons_container, BUTTON_OUTLINE, "No");
    Widget *button_yes = button_create_with_text(buttons_container, BUTTON_FILLED, "Yes");

    button_no->on(Event::ACTION, [&window](auto) {
        application_exit_nested(DIALOG_BUTTON_CLOSED);
        window_hide(window);
    });
    button_yes->on(Event::ACTION, [&window](auto) {
        application_exit_nested(DIALOG_BUTTON_YES);
        window_hide(window);
    });

    window_show(window);

    return application_run_nested();
}
