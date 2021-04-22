#include "libgraphic/Icon.h"
#include <libwidget/Spacer.h>
#include <libwidget/TitleBar.h>

#include <libfilepicker/dialogs/FilePicker.h>
#include <libfilepicker/widgets/DirectoryBrowser.h>
#include <libfilepicker/widgets/FileBrowser.h>
#include <libfilepicker/widgets/ToolBar.h>

namespace FilePicker
{

Dialog::Dialog(DialogFlags flags) : _flags(flags)
{
    _navigation = make<Navigation>();
    _navigation->go_home_dont_record_history();
    buttons(Widget::DialogButton::OK | Widget::DialogButton::CANCEL);
}

Dialog::~Dialog()
{
}

String Dialog::get_title()
{
    StringBuilder builder;
    if (_flags & DialogFlags::DIALOG_FLAGS_OPEN)
    {
        builder.append("Open ");
    }
    else if (_flags & DialogFlags::DIALOG_FLAGS_SAVE)
    {
        builder.append("Save ");
    }

    if (_flags & DialogFlags::DIALOG_FLAGS_FOLDER)
    {
        builder.append("folder");
    }
    else
    {
        builder.append("document");
    }

    return builder.finalize();
}

void Dialog::render(Widget::Window *window)
{
    window->size(Math::Vec2i(600, 400));
    window->root()->layout(VFLOW(0));

    new Widget::TitleBar(
        window->root(),
        Graphic::Icon::get("widgets"),
        get_title());

    new ToolBar(window->root(), _navigation, nullptr, ToolBar::NO_OPEN_TERMINAL);

    if (_flags & DialogFlags::DIALOG_FLAGS_FOLDER)
    {
        _browser = new DirectoryBrowser(window->root(), _navigation);
    }
    else
    {
        auto file_browser = new FileBrowser(window->root(), _navigation);
        file_browser->on_element_selected = [this](String &path) {
            _selected_file = path;
            close(Widget::DialogResult::OK);
        };
        _browser = file_browser;
    }

    auto action_container = new Widget::Panel(window->root());

    action_container->layout(HFLOW(4));

    if (_flags & DialogFlags::DIALOG_FLAGS_SAVE)
    {
        _text_field = new Widget::TextField(action_container, Widget::TextModel::empty());
        _text_field->flags(Widget::Component::FILL);
        _text_field->focus();
    }

    action_container->insets(Insetsi(4, 4));

    new Widget::Spacer(action_container);

    create_buttons(action_container);
}

void Dialog::on_button(Widget::DialogButton btn)
{
    if (btn == Widget::DialogButton::OK)
    {
        if (_flags & DialogFlags::DIALOG_FLAGS_OPEN)
        {
            _selected_file = _browser->selected_path();
        }
        else
        {
            _selected_file = IO::Path::join(_navigation->current(), _text_field->text()).string();
        }
    }
}

} // namespace FilePicker
