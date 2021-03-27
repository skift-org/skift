#pragma once

#include <libwidget/Container.h>
#include <libwidget/TitleBar.h>
#include <libwidget/Window.h>

#include <libfilepicker/model/Navigation.h>
#include <libfilepicker/widgets/DirectoryBrowser.h>
#include <libfilepicker/widgets/JumpList.h>
#include <libfilepicker/widgets/ToolBar.h>

class MainWindow :
    public Widget::Window
{
private:
public:
    MainWindow(RefPtr<FilePicker::Navigation> navigation, RefPtr<FilePicker::Bookmarks> bookmarks)
        : Widget::Window(WINDOW_RESIZABLE)
    {
        icon(Graphic::Icon::get("folder"));
        title("File Manager");
        size(Vec2i(700, 500));

        root()->layout(VFLOW(0));

        new Widget::TitleBar(root());

        new FilePicker::ToolBar(root(), navigation, bookmarks);

        auto bookmarks_and_browser = new Widget::Container(root());

        bookmarks_and_browser->flags(Widget::Component::FILL);
        bookmarks_and_browser->layout(HFLOW(1));

        auto jump_list = new FilePicker::JumpList(bookmarks_and_browser, navigation, bookmarks);

        jump_list->min_width(160);

        auto browser = new FilePicker::DirectoryBrowser(bookmarks_and_browser, navigation);

        browser->on_element_selected = [&](String &path) {
            auto l = launchpad_create("open", "/System/Utilities/open");
            launchpad_argument(l, path.cstring());
            launchpad_launch(l, nullptr);
        };
    }
};
