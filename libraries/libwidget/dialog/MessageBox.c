#include <libwidget/Application.h>
#include <libwidget/Widgets.h>
#include <libwidget/Window.h>
#include <libwidget/dialog/Dialog.h>

static void on_dialog_close(void *target, struct Window *sender, struct Event *event)
{
    __unused(target);
    __unused(event);

    application_exit_nested(DIALOG_BUTTON_CLOSED);
    window_hide(sender);
}

DialogButton dialog_message(
    Icon *icon,
    const char *title,
    const char *message,
    DialogButton buttons)
{
    __unused(buttons);

    Window *window = window_create(WINDOW_NONE);

    window_set_icon(window, icon);
    window_set_title(window, title);
    window_set_size(window, vec2i(300, 200));
    window_root(window)->layout = VFLOW(0);
    window_root(window)->insets = INSETS(8);

    window_set_event_handler(window, EVENT_WINDOW_CLOSING, EVENT_HANDLER(NULL, on_dialog_close));

    Widget *message_label = label_create(window_root(window), message);
    message_label->layout_attributes = LAYOUT_FILL;

    Widget *buttons_container = container_create(window_root(window));
    buttons_container->layout = HFLOW(4);
    container_create(buttons_container)->layout_attributes = LAYOUT_FILL;
    button_create_with_text(buttons_container, BUTTON_OUTLINE, "No");
    button_create_with_text(buttons_container, BUTTON_FILLED, "Yes");

    window_show(window);

    return application_run_nested();
}
