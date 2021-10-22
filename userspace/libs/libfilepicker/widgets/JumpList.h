#pragma once

#include <libwidget/Elements.h>
#include <libwidget/Layouts.h>

#include <libfilepicker/model/Bookmarks.h>
#include <libfilepicker/model/Navigation.h>

namespace FilePicker
{

struct JumpList : public Widget::Component
{
private:
    Ref<Navigation> _navigation;
    Ref<Bookmarks> _bookmarks;

public:
    JumpList(Ref<Navigation> navigation, Ref<Bookmarks> bookmarks)
        : _navigation(navigation),
          _bookmarks(bookmarks)
    {
    }

    Ref<Element> build()
    {
        Vec<Ref<Element>> children;

        for (size_t i = 0; i < _bookmarks->all().count(); i++)
        {
            auto bookmark = _bookmarks->all()[i];

            children.push_back(Widget::basic_button(bookmark.icon(), bookmark.name(), [this, bookmark]
                { _navigation->navigate(bookmark.path()); }));
        }

        return Widget::observer(
            *_bookmarks,
            [&]
            {
                return Widget::vflow({
                    Widget::label("Bookmarks"),
                    Widget::scroll(Widget::vflow(children)),
                });
            });
    }
};

} // namespace FilePicker