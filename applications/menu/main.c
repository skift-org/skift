#include <libwidget/Application.h>
#include <libwidget/Panel.h>

int main(int argc, char **argv)
{
    application_initialize(argc, argv);

    Window *window = window_create(NULL, "Panel", 320, 768, WINDOW_BORDERLESS);

    window_root(window)->layout = (Layout){LAYOUT_VFLOW, 8, 0};

    panel_create(window_root(window));

    window_show(window);

    return application_run();
}
