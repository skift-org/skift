#include <libsystem/Logger.h>
#include <libwidget/Application.h>
#include <libwidget/Button.h>
#include <libwidget/Panel.h>
#include <libwidget/TextEditor.h>
#include <libwidget/TitleBar.h>

int main(int argc, char **argv)
{
    Widget::Application::initialize(argc, argv);

    Widget::Window *window = new Widget::Window(WINDOW_RESIZABLE);

    window->icon(Graphic::Icon::get("text-box"));

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

    new Widget::TitleBar(window->root());

    auto toolbar = new Widget::Panel(window->root());

    toolbar->layout(HFLOW(4));
    toolbar->insets(Insetsi(4, 4));
    toolbar->max_height(38);
    toolbar->min_height(38);

    new Widget::Button(toolbar, Widget::Button::TEXT, Graphic::Icon::get("folder-open"));
    new Widget::Button(toolbar, Widget::Button::TEXT, Graphic::Icon::get("content-save"));
    new Widget::Button(toolbar, Widget::Button::TEXT, Graphic::Icon::get("file-plus"));

    auto model = Widget::TextModel::empty();

    if (argc == 2)
    {
        logger_info("Opening text document from '%s'", argv[1]);
        model = Widget::TextModel::from_file(argv[1]);
    }

    auto field = new Widget::TextEditor(window->root(), model);
    field->flags(Widget::Component::FILL);
    field->overscroll(true);
    field->insets({4});
    field->focus();

    window->show();

    return Widget::Application::run();
}
