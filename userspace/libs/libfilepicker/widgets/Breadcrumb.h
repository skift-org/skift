#pragma once

#include <libwidget/Element.h>

#include <libfilepicker/model/Bookmarks.h>
#include <libfilepicker/model/Navigation.h>

namespace FilePicker
{

struct Breadcrumb : public Widget::Element
{
private:
    Ref<Navigation> _navigation;
    Ref<Bookmarks> _bookmarks;

    Box<Async::Observer<Navigation>> _navigation_observer;
    Box<Async::Observer<Bookmarks>> _bookmarks_observer;

    Ref<Graphic::Icon> _icon_computer;
    Ref<Graphic::Icon> _icon_expand;
    Ref<Graphic::Icon> _icon_bookmark;
    Ref<Graphic::Icon> _icon_bookmark_outline;

public:
    Breadcrumb(Ref<Navigation> navigation, Ref<Bookmarks> bookmarks);

    void render();
};

} // namespace FilePicker