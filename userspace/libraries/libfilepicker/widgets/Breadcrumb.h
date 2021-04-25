#pragma once

#include <libwidget/Element.h>

#include <libfilepicker/model/Bookmarks.h>
#include <libfilepicker/model/Navigation.h>

namespace FilePicker
{

struct Breadcrumb : public Widget::Element
{
private:
    RefPtr<Navigation> _navigation;
    RefPtr<Bookmarks> _bookmarks;

    OwnPtr<Async::Observer<Navigation>> _navigation_observer;
    OwnPtr<Async::Observer<Bookmarks>> _bookmarks_observer;

    RefPtr<Graphic::Icon> _icon_computer;
    RefPtr<Graphic::Icon> _icon_expand;
    RefPtr<Graphic::Icon> _icon_bookmark;
    RefPtr<Graphic::Icon> _icon_bookmark_outline;

public:
    Breadcrumb(RefPtr<Navigation> navigation, RefPtr<Bookmarks> bookmarks);

    void render();
};

} // namespace FilePicker