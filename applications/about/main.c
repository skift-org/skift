#include <libsystem/BuildInfo.h>
#include <libwidget/Application.h>
#include <libwidget/Button.h>
#include <libwidget/Container.h>
#include <libwidget/Icon.h>
#include <libwidget/Label.h>

void ok_button_pressed(void *target, Widget *sender, Event *event)
{
    __unused(target);
    __unused(event);

    window_hide(sender->window);
}

int main(int argc, char **argv)
{
    application_initialize(argc, argv);

    Window *window = window_create("/res/icon/info.png", "About", 250, 250, WINDOW_NONE);

    window_root(window)->layout = (Layout){LAYOUT_VGRID, 0, 8};

    icon_create(window_root(window), "/res/skift.png");

    Widget *button_and_text = container_create(window_root(window));
    button_and_text->layout = (Layout){LAYOUT_VGRID, 0, 2};

    label_create(button_and_text, "The skift operating system.");
    label_create(button_and_text, __BUILD_GITREF__ "/" __BUILD_CONFIG__);
    label_create(button_and_text, "Copyright © 2018-2020 N. Van Bossuyt.");

    Widget *ok_buton = button_create(button_and_text, "Ok");

    widget_set_event_handler(ok_buton, EVENT_MOUSE_BUTTON_PRESS, NULL, ok_button_pressed);

    window_show(window);

    return application_run();
}
