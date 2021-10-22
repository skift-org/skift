#pragma once

#include <libwidget/Components.h>
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

struct Dialog : public ::Widget::Dialog
{
private:
    Ref<Navigation> _navigation = nullptr;
    Opt<String> _selected_file = NONE;
    Ref<Browser> _browser;
    DialogFlags _flags;
    Ref<Widget::TextField> _text_field;

    String get_title();

public:
    Dialog(DialogFlags flags = DIALOG_FLAGS_OPEN);

    virtual ~Dialog() {}

    Opt<String> selected_file() const { return _selected_file; }

    virtual void render(Widget::Window *window) override;
    virtual void on_button(Widget::DialogButton btn) override;
};

} // namespace FilePicker