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

    Widget::VScroll *_listing;

public:
    JumpList(Widget::Component *parent, RefPtr<Navigation> navigation, RefPtr<Bookmarks> bookmarks)
        : Panel(parent),
          _navigation(navigation),
          _bookmarks(bookmarks)
    {
        layout(VFLOW(6));
        insets(Insetsi{4});

        _bookmark_observer = bookmarks->observe([this](auto &) {
            render();
        });

        new Widget::Label(this, "Bookmarks");

        _listing = new Widget::VScroll(this);
        _listing->flags(Component::FILL);

        render();
    }

    void render()
    {
        _listing->host()->clear_children();
        _listing->host()->layout(VFLOW(4));

        for (size_t i = 0; i < _bookmarks->all().count(); i++)
        {
            auto bookmark = _bookmarks->all()[i];

            auto button = new Widget::Button(
                _listing->host(),
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