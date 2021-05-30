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
    RefPtr<Navigation> _navigation;
    RefPtr<Bookmarks> _bookmarks;

public:
    JumpList(RefPtr<Navigation> navigation, RefPtr<Bookmarks> bookmarks)
        : _navigation(navigation),
          _bookmarks(bookmarks)
    {
    }

    RefPtr<Element> build()
    {
        Vector<RefPtr<Element>> children;

        for (size_t i = 0; i < _bookmarks->all().count(); i++)
        {
            auto bookmark = _bookmarks->all()[i];

            children.push_back(Widget::basic_button(bookmark.icon(), bookmark.name(), [this, bookmark] {
                _navigation->navigate(bookmark.path());
            }));
        }

        return Widget::observer(
            *_bookmarks,
            [&] {
                return Widget::vflow({
                    Widget::label("Bookmarks"),
                    Widget::scroll(Widget::vflow(children)),
                });
            });
    }
};

} // namespace FilePicker