#include <libwidget/Application.h>
#include <libwidget/Panel.h>

int main(int argc, char **argv)
{
    application_initialize(argc, argv);

    Window *window = window_create("Widget Factory", 500, 400);

    window_root(window)->layout = LAYOUT_VFLOW;

    panel_create(window_root(window));
    panel_create(window_root(window));

    Widget *panel = panel_create(window_root(window));

    panel->layout = LAYOUT_HFLOW;

    panel_create(panel);
    panel_create(panel);
    panel_create(panel);
    panel_create(panel);

    panel_create(window_root(window));

    return application_run();
}
