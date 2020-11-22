#include <libwidget/Application.h>
#include <libwidget/Markup.h>
#include <libwidget/Widgets.h>

int main(int argc, char **argv)
{
    application_initialize(argc, argv);

    Window *window = window_create_from_file("/Applications/media-player/main.markup");

    window->with_widget<Slider>("volume_slider", [&](Slider *slider) {
        slider->on(Event::VALUE_CHANGE, [&](auto) {
            logger_trace("Value is %d", (int)(slider->value() * 100));
        });
    });

    window->with_widget<Image>("cover", [&](Image *image) {
        image->change_bitmap(Bitmap::load_from_or_placeholder("/Applications/media-player/cover.png"));
    });

    window->show();

    return application_run();
}
