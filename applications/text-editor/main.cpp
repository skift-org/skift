#include <libwidget/Application.h>
#include <libwidget/widgets/TextField.h>
#include <libwidget/widgets/Toolbar.h>

int main(int argc, char **argv)
{
    application_initialize(argc, argv);

    Window *window = window_create(WINDOW_RESIZABLE);

    window_set_icon(window, Icon::get("text-box"));
    window_set_title(window, "Text Editor");

    window_root(window)->layout(VFLOW(0));

    toolbar_create(window_root(window));

    auto field = new TextField(window_root(window));

    field->attributes(LAYOUT_FILL);

    window_show(window);

    return application_run();
}
