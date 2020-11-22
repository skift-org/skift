#include <libgraphic/Painter.h>
#include <libwidget/Window.h>
#include <libwidget/widgets/Button.h>
#include <libwidget/widgets/IconPanel.h>

#include "file-manager/widgets/Breadcrumb.h"

namespace file_manager
{

Breadcrumb::Breadcrumb(Widget *parent, RefPtr<Navigation> navigation)
    : Widget(parent),
      _navigation(navigation)
{
    layout(HFLOW(0));

    _icon_computer = Icon::get("laptop");
    _icon_expand = Icon::get("chevron-right");

    _observer = _navigation->observe([&](auto &) {
        clear_children();

        auto &path = _navigation->current();

        auto computer_button = new Button(this, ButtonStyle::BUTTON_TEXT, _icon_computer);

        computer_button->on(Event::ACTION, [&](auto) {
            _navigation->navigate("/");
        });

        for (size_t i = 0; i < path.length(); i++)
        {
            new IconPanel(this, _icon_expand);

            auto button = new Button(this, ButtonStyle::BUTTON_TEXT, path[i]);
            button->min_width(0);
            button->on(Event::ACTION, [&, i](auto) {
                _navigation->navigate(_navigation->current().parent(i), Navigation::BACKWARD);
            });
        }
    });
}

} // namespace file_manager
