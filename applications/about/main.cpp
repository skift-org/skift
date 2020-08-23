#include <libsystem/BuildInfo.h>
#include <libwidget/Application.h>
#include <libwidget/Markup.h>
#include <libwidget/Widgets.h>

int main(int argc, char **argv)
{
    application_initialize(argc, argv);

    Window *window = window_create_from_file("/Applications/about/about.markup");

    window->type(WINDOW_TYPE_DIALOG);

    Image *system_image = nullptr;
    if ((system_image = (Image *)window_get_widget_by_id(window, "system-image")))
    {
        if (theme_is_dark())
        {
            system_image->change_bitmap(Bitmap::load_from_or_placeholder("/Applications/about/logo-white.png"));
        }
        else
        {
            system_image->change_bitmap(Bitmap::load_from_or_placeholder("/Applications/about/logo-black.png"));
        }
    }

    Label *version_label = nullptr;
    if ((version_label = (Label *)window_get_widget_by_id(window, "version-label")))
    {
        version_label->text(__BUILD_GITREF__ "/" __BUILD_CONFIG__);
    }

    Widget *ok_button = nullptr;
    if ((ok_button = window_get_widget_by_id(window, "ok-button")))
    {
        ok_button->on(Event::ACTION, [](auto) {
            application_exit(0);
        });
    }

    window_show(window);

    return application_run();
}
