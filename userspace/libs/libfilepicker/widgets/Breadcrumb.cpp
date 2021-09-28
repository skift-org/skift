#include <libgraphic/Painter.h>

#include <libwidget/Elements.h>
#include <libwidget/Window.h>

#include <libfilepicker/widgets/Breadcrumb.h>

namespace FilePicker
{

Breadcrumb::Breadcrumb(RefPtr<Navigation> navigation, RefPtr<Bookmarks> bookmarks)
    : _navigation(navigation),
      _bookmarks(bookmarks)
{
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
    clear();

    auto &path = _navigation->current();

    auto computer_button = add(Widget::basic_button(_icon_computer));

    computer_button->on(Widget::Event::ACTION, [this](auto) {
        _navigation->navigate("/");
    });

    for (size_t i = 0; i < path.length(); i++)
    {
        add(Widget::icon(_icon_expand));

        auto button = add(Widget::basic_button(path[i], [this, i] {
            _navigation->navigate(_navigation->current().parent(i), Navigation::BACKWARD);
        }));

        button->min_width(0);
    }

    add(Widget::spacer());

    if (_bookmarks)
    {
        if (_bookmarks->has(_navigation->current()))
        {
            add(Widget::basic_button(_icon_bookmark, [this] {
                _bookmarks->remove(_navigation->current());
            }));
        }
        else
        {
            add(Widget::basic_button(_icon_bookmark_outline, [this] {
                Bookmark bookmark{
                    _navigation->current().basename(),
                    Graphic::Icon::get("folder"),
                    _navigation->current(),
                };

                _bookmarks->add(std::move(bookmark));
            }));
        }
    }
}

} // namespace FilePicker