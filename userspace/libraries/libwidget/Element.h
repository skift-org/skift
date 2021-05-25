#pragma once

#include <libgraphic/Font.h>
#include <libmath/Rect.h>
#include <libutils/Array.h>
#include <libutils/Assert.h>
#include <libwidget/Cursor.h>
#include <libwidget/Event.h>
#include <libwidget/Theme.h>

namespace Graphic
{

struct Painter;

} // namespace Graphic

namespace Widget
{

struct Window;

#define WIDGET_BUILDER(__type, __name)                         \
    template <typename... TArgs>                               \
    ::RefPtr<::Widget::Element> __name(TArgs &&...args)        \
    {                                                          \
        return ::make<__type>(::std::forward<TArgs>(args)...); \
    }

struct Element : public RefCounted<Element>
{
private:
    bool _enabled = true;
    int _flags = 0;

    Math::Recti _container;

    int _max_height = 0;
    int _max_width = 0;
    int _min_height = 0;
    int _min_width = 0;

    Math::Vec2i _content_scroll{};

    Array<Optional<Graphic::Color>, __THEME_COLOR_COUNT> _colors = {NONE};
    RefPtr<Graphic::Font> _font;

    CursorState _cursor = CURSOR_DEFAULT;

    EventHandler _handlers[EventType::__COUNT] = {};

    Element *_parent = {};
    Window *_window = {};

    Vector<RefPtr<Element>> _children = {};

public:
    static constexpr auto FILL = (1 << 0);
    static constexpr auto GREEDY = (1 << 1);
    static constexpr auto SQUARE = (1 << 2);
    static constexpr auto NO_MOUSE_HIT = (1 << 3);
    static constexpr auto NOT_AFFECTED_BY_SCROLL = (1 << 4);

    RefPtr<Graphic::Font> font()
    {
        if (!_font)
        {
            _font = Graphic::Font::get("sans").unwrap();
        }

        return _font;
    }

    void font(RefPtr<Graphic::Font> font)
    {
        _font = font;
    }

    Graphic::Color color(ThemeColorRole role);

    void color(ThemeColorRole role, Graphic::Color color);

    void flags(int attributes) { _flags = attributes; }

    int flags() { return _flags; }

    Window *window()
    {
        Assert::not_null(_window);
        return _window;
    }

    void min_height(int value) { _min_height = value; }
    int min_height() { return _min_height; }

    void max_height(int value) { _max_height = value; }
    int max_height() { return _max_height; }

    void pin_height(int value)
    {
        _min_height = value;
        _max_height = value;
    }

    void min_width(int value) { _min_width = value; }
    int min_width() { return _min_width; }

    void max_width(int value) { _max_width = value; }
    int max_width() { return _max_width; }

    void pin_width(int value)
    {
        _min_width = value;
        _max_width = value;
    }

    CursorState cursor();

    void cursor(CursorState cursor);

    /* --- subclass API ----------------------------------------------------- */

    Element();

    virtual ~Element();

    virtual void paint(Graphic::Painter &, const Math::Recti &) {}

    virtual void event(Event *) {}

    virtual void mounted() {}

    virtual void layout();

    virtual Math::Vec2i size();

    /* --- Metrics ---------------------------------------------------------- */

    Math::Vec2i position_in_window()
    {
        if (_parent)
        {
            return _parent->position_in_window() + origin();
        }
        else
        {
            return origin();
        }
    }

    Math::Recti container() const { return _container; }

    void container(Math::Recti container) { _container = container; }

    Math::Vec2i origin() const
    {
        if (_parent && !(_flags & NOT_AFFECTED_BY_SCROLL))
        {
            return {
                container().x() - _parent->scroll().x(),
                container().y() - _parent->scroll().y(),
            };
        }
        else
        {
            return container().position();
        }
    }

    Math::Recti bound() const { return container().size(); }

    Math::Vec2i scroll() { return _content_scroll; }

    void scroll(Math::Vec2i content_scroll)
    {
        _content_scroll = content_scroll;
        should_repaint();
    }

    /* --- Enable/Disable state --------------------------------------------- */

    bool enabled();

    bool disabled();

    void enable();

    void disable();

    void disable_if(bool condition);

    void enable_if(bool condition);

    /* --- Children --------------------------------------------------------- */

    Element *at(Math::Vec2i position);

    const Vector<RefPtr<Element>> &children() { return _children; }

    template <typename T, typename... Args>
    RefPtr<T> add(Args &&...args)
    {
        return add(make<T>(std::forward<Args>(args)...));
    }

    void add(Vector<RefPtr<Element>> children)
    {
        for (auto &child : children)
        {
            add(child);
        }
    }

    RefPtr<Element> add(RefPtr<Element> child);

    void del(RefPtr<Element> child);

    void mount(Element &parent);

    void mount(Window &window);

    void unmount();

    void clear();

    /* --- Focus state ------------------------------------------------------ */

    bool focused();

    void focus();

    /* --- Layout ----------------------------------------------------------- */

    void relayout();

    void should_relayout();

    Math::Vec2i compute_size();

    /* --- Paint ------------------------------------------------------------ */

    void repaint(Graphic::Painter &painter, Math::Recti rectangle);

    void should_repaint();

    void should_repaint(Math::Recti rectangle);

    /* --- Events ----------------------------------------------------------- */

    void on(EventType event, EventHandler handler);

    void dispatch_event(Event *event);
};

} // namespace Widget
