#include <libgraphic/Painter.h>

#include <libwidget/Button.h>
#include <libwidget/IconPanel.h>
#include <libwidget/Spacer.h>
#include <libwidget/Window.h>

#include <libfilepicker/widgets/Breadcrumb.h>

namespace FilePicker
{

Breadcrumb::Breadcrumb(Widget::Component *parent, RefPtr<Navigation> navigation, RefPtr<Bookmarks> bookmarks)
    : Widget::Component(parent),
      _navigation(navigation),
      _bookmarks(bookmarks)
{
    layout(HFLOW(0));

    _icon_computer = Graphic::Icon::get("laptop");
    _icon_expand = Graphic::Icon::get("chevron-right");
    _icon_bookmark = Graphic::Icon::get("bookmark");
    _icon_bookmark_outline = Graphic::Icon::get("bookmark-outline");

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

    auto computer_button = new Widget::Button(this, Widget::Button::TEXT, _icon_computer);

    computer_button->on(Widget::Event::ACTION, [this](auto) {
        _navigation->navigate("/");
    });

    for (size_t i = 0; i < path.length(); i++)
    {
        new Widget::IconPanel(this, _icon_expand);

        auto button = new Widget::Button(this, Widget::Button::TEXT, path[i]);
        button->min_width(0);

        button->on(Widget::Event::ACTION, [this, i](auto) {
            _navigation->navigate(_navigation->current().parent(i), Navigation::BACKWARD);
        });
    }

    new Widget::Spacer(this);

    if (_bookmarks)
    {
        if (_bookmarks->has(_navigation->current()))
        {
            auto remove_bookmark = new Widget::Button(this, Widget::Button::TEXT, _icon_bookmark);

            remove_bookmark->on(Widget::Event::ACTION, [this](auto) {
                _bookmarks->remove(_navigation->current());
            });
        }
        else
        {
            auto add_bookmark = new Widget::Button(this, Widget::Button::TEXT, _icon_bookmark_outline);

            add_bookmark->on(Widget::Event::ACTION, [this](auto) {
                Bookmark bookmark{
                    _navigation->current().basename(),
                    Graphic::Icon::get("folder"),
                    _navigation->current(),
                };

                _bookmarks->add(move(bookmark));
            });
        }
    }
}

} // namespace FilePicker