#pragma once

#include <libwidget/TextField.h>
#include <libwidget/dialog/Dialog.h>

#include <libfilepicker/model/Navigation.h>
#include <libfilepicker/widgets/Browser.h>

namespace FilePicker
{

enum DialogFlags
{
    DIALOG_FLAGS_OPEN = 1,
    DIALOG_FLAGS_SAVE = 2,
    DIALOG_FLAGS_FOLDER = 4,
    DIALOG_FLAGS_MULTIPLE = 8
};

class Dialog : public ::Widget::Dialog
{
private:
    RefPtr<Navigation> _navigation = nullptr;
    Optional<String> _selected_file{};
    RefPtr<Browser> _browser;
    DialogFlags _flags;
    RefPtr<Widget::TextField> _text_field;

    String get_title();

public:
    Dialog(DialogFlags flags = DIALOG_FLAGS_OPEN);

    ~Dialog();

    Optional<String> selected_file() const { return _selected_file; }

    virtual void render(Widget::Window *window) override;
    virtual void on_button(Widget::DialogButton btn) override;
};

} // namespace FilePicker