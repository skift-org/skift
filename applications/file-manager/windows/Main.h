#pragma once

#include <libwidget/Window.h>
#include <libwidget/widgets/Container.h>

#include "file-manager/model/Navigation.h"
#include "file-manager/widgets/Browser.h"
#include "file-manager/widgets/JumpList.h"
#include "file-manager/widgets/ToolBar.h"

namespace file_manager
{

class Main:
    public Window
{
private:
public:
    Main(RefPtr<Navigation> navigation, RefPtr<Bookmarks> bookmarks) :
        Window(WINDOW_RESIZABLE)
    {
        icon(Icon::get("folder"));
        title("File Manager");
        size(Vec2i(700, 500));

        root()->layout(VFLOW(0));

        new ToolBar(root(), navigation, bookmarks);

        auto bookmarks_and_browser = new Container(root());

        bookmarks_and_browser->attributes(LAYOUT_FILL);
        bookmarks_and_browser->layout(HFLOW(1));

        auto jump_list = new JumpList(bookmarks_and_browser, navigation, bookmarks);
        jump_list->min_width(160);

        new Browser(bookmarks_and_browser, navigation);
    }
};

} // namespace file_manager
