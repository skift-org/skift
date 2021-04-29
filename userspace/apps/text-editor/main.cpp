#include <libfilepicker/FilePicker.h>
#include <libio/File.h>
#include <libio/Streams.h>
#include <libwidget/Application.h>
#include <libwidget/Components.h>
#include <libwidget/Elements.h>

struct TextWindow : public Widget::Window
{
private:
    /// --- Toolbar --- ///
    RefPtr<Widget::Element> _open_document;
    RefPtr<Widget::Element> _save_document;
    RefPtr<Widget::Element> _new_document;

    RefPtr<Widget::TextModel> _text_model;
    RefPtr<Widget::TitleBarComponent> _title_bar;
    RefPtr<Widget::TextEditor> _text_editor;

    void load_document(String path)
    {
        if (!path.null_or_empty())
        {
            IO::logln("Opening text document from {}", path);
            _title_bar->title(path);
            _text_model = Widget::TextModel::open(path);
            _text_editor->update_model(_text_model);
        }
    }

    void save_document(String path)
    {
        if (!path.null_or_empty())
        {
            IO::logln("Saving text document to {}", path);
            _title_bar->title(path);
            _text_model->save(path);
        }
    }

public:
    TextWindow(const char *path) : Window(WINDOW_RESIZABLE)
    {
        size(Math::Vec2i(700, 500));

        root()->layout(VFLOW(0));

        _title_bar = Widget::titlebar(Graphic::Icon::get("text-box"), "Text Editor");
        root()->add(_title_bar);

        create_toolbar(root());

        _text_model = Widget::TextModel::empty();

        _text_editor = root()->add<Widget::TextEditor>(_text_model);
        _text_editor->flags(Widget::Element::FILL);
        _text_editor->overscroll(true);
        _text_editor->insets({4});
        _text_editor->focus();

        load_document(path);
    }

    void create_toolbar(RefPtr<Widget::Element> parent)
    {
        auto toolbar = parent->add(Widget::panel());

        toolbar->layout(HFLOW(4));
        toolbar->insets(Insetsi(4, 4));

        _open_document = toolbar->add(Widget::basic_button(Graphic::Icon::get("folder-open"), [&] {
            FilePicker::Dialog picker{};

            if (picker.show() == Widget::DialogResult::OK)
            {
                load_document(picker.selected_file().unwrap());
            }
        }));

        _save_document = toolbar->add(Widget::basic_button(Graphic::Icon::get("content-save"), [&] {
            FilePicker::Dialog picker{FilePicker::DIALOG_FLAGS_SAVE};
            if (picker.show() == Widget::DialogResult::OK)
            {
                save_document(picker.selected_file().unwrap());
            }
        }));

        _new_document = toolbar->add(Widget::basic_button(Graphic::Icon::get("image-plus")));
    }
};

int main(int argc, char **argv)
{
    const char *path = argc > 1 ? argv[1] : "";
    auto window = new TextWindow(path);
    window->show();

    return Widget::Application::the()->run();
}
