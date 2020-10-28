#include <libwidget/Application.h>
#include <libwidget/Window.h>

#include "terminal/TerminalWidget.h"

int main(int argc, char **argv)
{
    application_initialize(argc, argv);

    Window *window = new Window(WINDOW_RESIZABLE);

    window->icon(Icon::get("console-line"));
    window->title("Terminal");
    window->size(Vec2i(700, 500));

    Widget *widget = new TerminalWidget(window->root());
    widget->focus();

    window->show();

    return application_run();
}
