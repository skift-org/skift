#include <libwidget/Spacer.h>
#include <libwidget/TitleBar.h>

#include <libfilepicker/dialogs/FilePicker.h>
#include <libfilepicker/widgets/DirectoryBrowser.h>
#include <libfilepicker/widgets/ToolBar.h>

namespace filepicker
{

Dialog::Dialog()
{
    _navigation = make<Navigation>();
    _navigation->go_home_dont_record_history();
    buttons(DialogButton::OK | DialogButton::CANCEL);
    title("File picker");
}

Dialog::~Dialog()
{
}

void Dialog::render(Window *window)
{
    window->size(Vec2i(600, 400));
    window->root()->layout(VFLOW(0));

    new TitleBar(window->root());

    new ToolBar(window->root(), _navigation, nullptr, ToolBar::NO_OPEN_TERMINAL);

    auto browser = new DirectoryBrowser(window->root(), _navigation);
    browser->on_element_selected = [this](String &path) {
        _selected_file = path;
        close(DialogResult::OK);
    };

    auto action_container = new Panel(window->root());

    action_container->layout(HFLOW(4));
    action_container->insets(Insetsi(4, 4));

    new Spacer(action_container);

    create_buttons(action_container);
}

} // namespace filepicker
