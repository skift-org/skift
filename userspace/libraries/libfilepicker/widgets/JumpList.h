#pragma once

#include <libwidget/Button.h>
#include <libwidget/Label.h>
#include <libwidget/Panel.h>
#include <libwidget/VScroll.h>

#include <libfilepicker/model/Bookmarks.h>
#include <libfilepicker/model/Navigation.h>

namespace FilePicker
{

class JumpList : public Widget::Panel
{
private:
    RefPtr<Navigation> _navigation;
    RefPtr<Bookmarks> _bookmarks;

    OwnPtr<Async::Observer<Bookmarks>> _bookmark_observer;

    RefPtr<Widget::VScroll> _listing;

public:
    JumpList(RefPtr<Navigation> navigation, RefPtr<Bookmarks> bookmarks)
        : Panel(),
          _navigation(navigation),
          _bookmarks(bookmarks)
    {
        layout(VFLOW(6));
        insets(Insetsi{4});

        _bookmark_observer = bookmarks->observe([this](auto &) {
            render();
        });

        add<Widget::Label>("Bookmarks");

        _listing = add<Widget::VScroll>();
        _listing->flags(Element::FILL);

        render();
    }

    void render()
    {
        _listing->host()->clear();
        _listing->host()->layout(VFLOW(4));

        for (size_t i = 0; i < _bookmarks->all().count(); i++)
        {
            auto bookmark = _bookmarks->all()[i];

            auto button = _listing->host()->add<Widget::Button>(
                Widget::Button::TEXT,
                bookmark.icon(),
                bookmark.name());

            button->on(Widget::Event::ACTION, [this, bookmark](auto) {
                _navigation->navigate(bookmark.path());
            });
        }
    }
};

} // namespace FilePicker