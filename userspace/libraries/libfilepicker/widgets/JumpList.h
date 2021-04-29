#pragma once

#include <libwidget/Elements.h>

#include <libfilepicker/model/Bookmarks.h>
#include <libfilepicker/model/Navigation.h>

namespace FilePicker
{

class JumpList : public Widget::PanelElement
{
private:
    RefPtr<Navigation> _navigation;
    RefPtr<Bookmarks> _bookmarks;

    OwnPtr<Async::Observer<Bookmarks>> _bookmark_observer;

    RefPtr<Widget::ScrollElement> _listing;

public:
    JumpList(RefPtr<Navigation> navigation, RefPtr<Bookmarks> bookmarks)
        : PanelElement(),
          _navigation(navigation),
          _bookmarks(bookmarks)
    {
        DONT_USE_ME_layout(VFLOW(6));
        insets(Insetsi{4});

        _bookmark_observer = bookmarks->observe([this](auto &) {
            render();
        });

        add(Widget::label("Bookmarks"));

        _listing = Widget::fill(Widget::scroll());
        add(_listing);

        render();
    }

    void render()
    {
        _listing->host()->clear();
        _listing->host()->DONT_USE_ME_layout(VFLOW(4));

        for (size_t i = 0; i < _bookmarks->all().count(); i++)
        {
            auto bookmark = _bookmarks->all()[i];

            _listing->host()->add(Widget::basic_button(bookmark.icon(), bookmark.name(), [this, bookmark] {
                _navigation->navigate(bookmark.path());
            }));
        }
    }
};

} // namespace FilePicker