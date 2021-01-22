#pragma once

#include <libwidget/Button.h>
#include <libwidget/Label.h>
#include <libwidget/Panel.h>

#include <libfilepicker/model/Bookmarks.h>
#include <libfilepicker/model/Navigation.h>

namespace filepicker
{

class JumpList : public Panel
{
private:
    RefPtr<Navigation> _navigation;
    RefPtr<Bookmarks> _bookmarks;

    OwnPtr<Observer<Bookmarks>> _bookmark_observer;

public:
    JumpList(Widget *parent, RefPtr<Navigation> navigation, RefPtr<Bookmarks> bookmarks)
        : Panel(parent),
          _navigation(navigation),
          _bookmarks(bookmarks)
    {
        layout(VFLOW(4));
        insets(Insetsi{4});

        _bookmark_observer = bookmarks->observe([this](auto &) {
            render();
        });

        render();
    }

    void render()
    {
        clear_children();

        new Label(this, "Bookmarks");

        for (size_t i = 0; i < _bookmarks->all().count(); i++)
        {
            auto bookmark = _bookmarks->all()[i];

            auto button = new Button(
                this,
                Button::TEXT,
                bookmark.icon(),
                bookmark.name());

            button->on(Event::ACTION, [this, bookmark](auto) {
                _navigation->navigate(bookmark.path());
            });
        }
    }
};

} // namespace filepicker