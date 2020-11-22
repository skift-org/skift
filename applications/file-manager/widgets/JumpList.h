#pragma once

#include <libwidget/widgets/Button.h>
#include <libwidget/widgets/Label.h>
#include <libwidget/widgets/Panel.h>

#include "file-manager/model/Bookmarks.h"
#include "file-manager/model/Navigation.h"

namespace file_manager
{

class JumpList: public Panel
{
private:
    RefPtr<Navigation> _navigation;
    RefPtr<Bookmarks> _bookmarks;

public:
    JumpList(
        Widget *parent,
        RefPtr<Navigation> navigation,
        RefPtr<Bookmarks> bookmarks)
        : Panel(parent),
          _navigation(navigation),
          _bookmarks(bookmarks)
    {
        layout(VFLOW(4));
        insets(Insets{4});
        update();
    }

    void update()
    {
        clear_children();

        new Label(this, "Bookmarks");

        for (size_t i = 0; i < _bookmarks->all().count(); i++)
        {
            auto &b = _bookmarks->all()[i];

            auto button = new Button(this, ButtonStyle::BUTTON_TEXT, b.icon, b.name);
            button->on(Event::ACTION, [&](auto) {
                _navigation->navigate(b.path, Navigation::BACKWARD);
            });
        }
    }
};

} // namespace file_manager
