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

void Dialog::render(Widget::Window *window)
{
    window->size(Math::Vec2i(600, 400));
    window->root()->layout(VFLOW(0));

    new Widget::TitleBar(
        window->root(),
        Graphic::Icon::get("widgets"),
        "File picker");

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
    action_container->insets(Insetsi(4, 4));

    new Widget::Spacer(action_container);

    create_buttons(action_container);
}

void Dialog::on_button(Widget::DialogButton btn)
{
    if (btn == Widget::DialogButton::OK)
    {
        _selected_file = _browser->selected_path();
    }
}

} // namespace FilePicker
