#pragma once

#include <libwidget/Widget.h>

#include "file-manager/model/Bookmarks.h"
#include "file-manager/model/Navigation.h"

namespace file_manager
{

struct Breadcrumb: public Widget
{
private:
    RefPtr<Navigation> _navigation;
    RefPtr<Bookmarks> _bookmarks;

    OwnPtr<Observer<Navigation>> _navigation_observer;
    OwnPtr<Observer<Bookmarks>> _bookmarks_observer;

    RefPtr<Icon> _icon_computer;
    RefPtr<Icon> _icon_expand;
    RefPtr<Icon> _icon_bookmark;
    RefPtr<Icon> _icon_bookmark_outline;

public:
    Breadcrumb(Widget *parent, RefPtr<Navigation> navigation, RefPtr<Bookmarks> bookmarks);

    void render();
};

} // namespace file_manager
