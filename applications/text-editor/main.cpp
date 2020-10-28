#include <libwidget/Application.h>
#include <libwidget/widgets/TextField.h>
#include <libwidget/widgets/Toolbar.h>

int main(int argc, char **argv)
{
    application_initialize(argc, argv);

    Window *window = new Window(WINDOW_RESIZABLE);

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

    window->root()->layout(VFLOW(0));

    auto toolbar = toolbar_create(window->root());
    toolbar_icon_create(toolbar, Icon::get("folder-open"));
    toolbar_icon_create(toolbar, Icon::get("content-save"));
    toolbar_icon_create(toolbar, Icon::get("file-plus"));

    auto model = TextModel::empty();

    if (argc == 2)
    {
        logger_info("Opening text document from '%s'", argv[1]);
        model = TextModel::from_file(argv[1]);
    }

    auto field = new TextField(window->root(), model);
    field->attributes(LAYOUT_FILL);
    field->overscroll(true);
    field->linenumbers(true);
    field->focus();

    window->show();

    return application_run();
}
