#include <libwidget/Application.h>
#include <libwidget/widgets/TextField.h>
#include <libwidget/widgets/Toolbar.h>

int main(int argc, char **argv)
{
    application_initialize(argc, argv);

    Window *window = window_create(WINDOW_RESIZABLE);

    window->icon(Icon::get("text-box"));

    if (argc == 2)
    {
        window->title(argv[1]);
    }
    else
    {
        window->title("Text Editor");
    }

    window->size(Vec2i(700, 500));

    window_root(window)->layout(VFLOW(0));

    auto toolbar = toolbar_create(window_root(window));
    toolbar_icon_create(toolbar, Icon::get("folder-open"));
    toolbar_icon_create(toolbar, Icon::get("content-save"));
    toolbar_icon_create(toolbar, Icon::get("file-plus"));

    auto model = make<TextModel>();

    if (argc == 2)
    {
        model = TextModel::from_file(argv[1]);
    }

    auto field = new TextField(window_root(window), model);
    field->attributes(LAYOUT_FILL);
    field->focus();

    window_show(window);

    return application_run();
}
