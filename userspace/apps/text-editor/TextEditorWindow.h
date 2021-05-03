#include <libfilepicker/FilePicker.h>
#include <libio/Streams.h>
#include <libwidget/Components.h>
#include <libwidget/Layouts.h>
#include <libwidget/Window.h>
#include <libwidget/model/TextModel.h>

using namespace Widget;

struct TextEditorWindow : public Window
{
private:
    RefPtr<TextModel> _text_model;

public:
    TextEditorWindow(String path) : Window(WINDOW_RESIZABLE)
    {
        _text_model = TextModel::empty();
        size(Math::Vec2i(700, 500));
        load_document(path);
    }

    void load_document(String path)
    {
        if (!path.null_or_empty())
        {
            IO::logln("Opening text document from {}", path);
            _text_model = TextModel::open(path);
            should_rebuild();
        }
    }

    void save_document(String path)
    {
        if (!path.null_or_empty())
        {
            IO::logln("Saving text document to {}", path);
            _text_model->save(path);
            should_rebuild();
        }
    }

    RefPtr<Element> build() override
    {
        return vflow({
            titlebar(Graphic::Icon::get("text-box"), "Text Editor"),

            toolbar({
                basic_button(Graphic::Icon::get("folder-open"), [&] {
                    FilePicker::Dialog picker{};

                    if (picker.show() == DialogResult::OK)
                    {
                        load_document(picker.selected_file().unwrap());
                    }
                }),
                basic_button(Graphic::Icon::get("content-save"), [&] {
                    FilePicker::Dialog picker{FilePicker::DIALOG_FLAGS_SAVE};
                    if (picker.show() == DialogResult::OK)
                    {
                        save_document(picker.selected_file().unwrap());
                    }
                }),
                basic_button(Graphic::Icon::get("image-plus")),
            }),

            texteditor(_text_model),
        });
    }
};
