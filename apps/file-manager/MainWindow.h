#pragma once

#include <libwidget/Container.h>
#include <libwidget/TitleBar.h>
#include <libwidget/Window.h>

#include <libfilepicker/model/Navigation.h>
#include <libfilepicker/widgets/DirectoryBrowser.h>
#include <libfilepicker/widgets/JumpList.h>
#include <libfilepicker/widgets/ToolBar.h>

class MainWindow :
    public Window
{
private:
public:
    MainWindow(RefPtr<filepicker::Navigation> navigation, RefPtr<filepicker::Bookmarks> bookmarks)
        : Window(WINDOW_RESIZABLE)
    {
        icon(Icon::get("folder"));
        title("File Manager");
        size(Vec2i(700, 500));

        root()->layout(VFLOW(0));

        new TitleBar(root());

        new filepicker::ToolBar(root(), navigation, bookmarks);

        auto bookmarks_and_browser = new Container(root());

        bookmarks_and_browser->flags(Widget::FILL);
        bookmarks_and_browser->layout(HFLOW(1));

        auto jump_list = new filepicker::JumpList(bookmarks_and_browser, navigation, bookmarks);

        jump_list->min_width(160);

        auto browser = new filepicker::DirectoryBrowser(bookmarks_and_browser, navigation);

        browser->on_element_selected = [&](String &path) {
            auto l = launchpad_create("open", "/System/Utilities/open");
            launchpad_argument(l, path.cstring());
            launchpad_launch(l, nullptr);
        };
    }
};
