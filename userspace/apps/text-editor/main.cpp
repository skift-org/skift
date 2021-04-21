#include <libfilepicker/FilePicker.h>
#include <libio/Streams.h>
#include <libwidget/Application.h>
#include <libwidget/Button.h>
#include <libwidget/Panel.h>
#include <libwidget/TextEditor.h>
#include <libwidget/TitleBar.h>

struct TextWindow : public Widget::Window
{
private:
    /// --- Toolbar --- ///
    Widget::Component *_open_document;
    Widget::Component *_save_document;
    Widget::Component *_new_document;

    RefPtr<Widget::TextModel> _text_model;
    Widget::TitleBar *_title_bar;
    Widget::TextEditor *_text_editor;

    FilePicker::Dialog _dialog;

    void load_document(String path)
    {
        if (!path.null_or_empty())
        {
            IO::logln("Opening text document from {}", path);
            _title_bar->title(path);
            _text_model = Widget::TextModel::from_file(path);
            _text_editor->update_model(_text_model);
        }
    }

public:
    TextWindow(const char *path) : Window(WINDOW_RESIZABLE)
    {
        size(Math::Vec2i(700, 500));

        root()->layout(VFLOW(0));

        _title_bar = new Widget::TitleBar(
            root(),
            Graphic::Icon::get("text-box"),
            "Text Editor");

        create_toolbar(root());

        _text_model = Widget::TextModel::empty();

        _text_editor = new Widget::TextEditor(root(), _text_model);
        _text_editor->flags(Widget::Component::FILL);
        _text_editor->overscroll(true);
        _text_editor->insets({4});
        _text_editor->focus();

        load_document(path);
    }

    void create_toolbar(Widget::Component *parent)
    {
        auto toolbar = new Widget::Panel(parent);

        toolbar->layout(HFLOW(4));
        toolbar->insets(Insetsi(4, 4));

        _open_document = new Widget::Button(toolbar, Widget::Button::TEXT, Graphic::Icon::get("folder-open"));
        _open_document->on(Widget::Event::ACTION, [&](auto) {
            if (_dialog.show() == Widget::DialogResult::OK)
            {
                load_document(_dialog.selected_file().unwrap());
            }
        });
        _save_document = new Widget::Button(toolbar, Widget::Button::TEXT, Graphic::Icon::get("content-save"));
        _save_document->on(Widget::Event::ACTION, [&](auto) {
            FilePicker::Dialog picker{};
            picker.show();
        });
        _new_document = new Widget::Button(toolbar, Widget::Button::TEXT, Graphic::Icon::get("image-plus"));
    }
};

int main(int argc, char **argv)
{
    Widget::Application::initialize(argc, argv);

    const char *path = argc > 1 ? argv[1] : "";
    auto window = new TextWindow(path);
    window->show();

    return Widget::Application::run();
}
