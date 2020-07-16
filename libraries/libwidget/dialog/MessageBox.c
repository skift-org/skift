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

    window_set_event_handler(window, EVENT_WINDOW_CLOSING, EVENT_HANDLER(NULL, on_dialog_close));

    label_create(window_root(window), message);

    window_show(window);

    return application_run_nested();
}
