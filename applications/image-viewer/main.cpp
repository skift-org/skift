#include <libwidget/Application.h>
#include <libwidget/Widgets.h>

int main(int argc, char **argv)
{
    if (argc == 1)
    {
        return -1;
    }

    if (application_initialize(argc, argv) != SUCCESS)
    {
        return -1;
    }

    Window *window = window_create(WINDOW_RESIZABLE);

    window_set_icon(window, icon_get("image"));
    window_set_title(window, "Image Viewer");
    window_set_size(window, Vec2i(700, 500));

    image_create(window_root(window), argv[1]);

    window_show(window);

    return application_run();
}
