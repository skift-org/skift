#include <libgraphic/Painter.h>

#include <libwidget/Button.h>
#include <libwidget/IconPanel.h>
#include <libwidget/Spacer.h>
#include <libwidget/Window.h>

#include <libfilepicker/widgets/Breadcrumb.h>

namespace filepicker
{

Breadcrumb::Breadcrumb(Widget *parent, RefPtr<Navigation> navigation, RefPtr<Bookmarks> bookmarks)
    : Widget(parent),
      _navigation(navigation),
      _bookmarks(bookmarks)
{
    layout(HFLOW(0));

    _icon_computer = Icon::get("laptop");
    _icon_expand = Icon::get("chevron-right");
    _icon_bookmark = Icon::get("bookmark");
    _icon_bookmark_outline = Icon::get("bookmark-outline");

    _navigation_observer = _navigation->observe([this](auto &) {
        render();
    });

    if (_bookmarks != nullptr)
    {
        _bookmarks_observer = _bookmarks->observe([this](auto &) {
            render();
        });
    }

    render();
}

void Breadcrumb::render()
{
    clear_children();

    auto &path = _navigation->current();

    auto computer_button = new Button(this, Button::TEXT, _icon_computer);

    computer_button->on(Event::ACTION, [this](auto) {
        _navigation->navigate("/");
    });

    for (size_t i = 0; i < path.length(); i++)
    {
        new IconPanel(this, _icon_expand);

        auto button = new Button(this, Button::TEXT, path[i]);
        button->min_width(0);

        button->on(Event::ACTION, [this, i](auto) {
            _navigation->navigate(_navigation->current().parent(i), Navigation::BACKWARD);
        });
    }

    new Spacer(this);

    if (_bookmarks)
    {
        if (_bookmarks->has(_navigation->current()))
        {
            auto remove_bookmark = new Button(this, Button::TEXT, _icon_bookmark);

            remove_bookmark->on(Event::ACTION, [this](auto) {
                _bookmarks->remove(_navigation->current());
            });
        }
        else
        {
            auto add_bookmark = new Button(this, Button::TEXT, _icon_bookmark_outline);

            add_bookmark->on(Event::ACTION, [this](auto) {
                Bookmark bookmark{
                    _navigation->current().basename(),
                    Icon::get("folder"),
                    _navigation->current(),
                };

                _bookmarks->add(move(bookmark));
            });
        }
    }
}

} // namespace filepicker