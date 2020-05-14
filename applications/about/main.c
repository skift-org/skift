#include <libsystem/BuildInfo.h>
#include <libwidget/Application.h>
#include <libwidget/Button.h>
#include <libwidget/Container.h>
#include <libwidget/Image.h>
#include <libwidget/Label.h>

void ok_button_pressed(void *target, Widget *sender, Event *event)
{
    __unused(target);
    __unused(event);
    __unused(sender);

    application_exit(-1);
}

Window *about_dialog_create(void)
{
    Window *dialog = window_create("information", "About", 250, 250, WINDOW_NONE);

    window_root(dialog)->layout = VGRID(8);
    window_root(dialog)->insets = INSETS(0, 8, 8);

    if (theme_is_dark())
    {
        image_create(window_root(dialog), "/res/skift-white.png");
    }
    else
    {
        image_create(window_root(dialog), "/res/skift-black.png");
    }

    Widget *button_and_text = container_create(window_root(dialog));
    button_and_text->layout = VGRID(2);

    label_create(button_and_text, "The skift operating system.");
    label_create(button_and_text, __BUILD_GITREF__ "/" __BUILD_CONFIG__);
    label_create(button_and_text, "Copyright © 2018-2020 N. Van Bossuyt.");

    Widget *ok_buton = button_create(button_and_text, "Ok");
    widget_set_event_handler(ok_buton, EVENT_MOUSE_BUTTON_PRESS, NULL, ok_button_pressed);

    return dialog;
}

int main(int argc, char **argv)
{
    application_initialize(argc, argv);

    window_show(about_dialog_create());

    return application_run();
}
