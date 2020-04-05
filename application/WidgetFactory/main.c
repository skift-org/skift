#include <libwidget/Application.h>
#include <libwidget/Panel.h>

int main(int argc, char **argv)
{
    application_initialize(argc, argv);

    Window *window = window_create("Widget Factory", 500, 400);

    window_root(window)->layout = LAYOUT_VFLOW;

    panel_create(window_root(window), (Rectangle){});
    panel_create(window_root(window), (Rectangle){});

    Widget *panel = panel_create(window_root(window), (Rectangle){});

    panel->layout = LAYOUT_HFLOW;

    panel_create(panel, (Rectangle){});
    panel_create(panel, (Rectangle){});
    panel_create(panel, (Rectangle){});
    panel_create(panel, (Rectangle){});

    panel_create(window_root(window), (Rectangle){});

    return application_run();
}
