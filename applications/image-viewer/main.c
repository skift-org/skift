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

    Window *window = window_create("image", "Image Viewer", 700, 500, WINDOW_RESIZABLE);

    image_create(window_root(window), argv[1]);

    window_show(window);

    return application_run();
}
