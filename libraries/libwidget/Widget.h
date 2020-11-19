#pragma once

#include <libgraphic/Font.h>
#include <libgraphic/Shape.h>
#include <libsystem/utils/List.h>
#include <libwidget/Event.h>
#include <libwidget/Theme.h>

struct Widget;
struct Painter;
struct Window;

enum LayoutType
{
    LAYOUT_STACK,
    LAYOUT_GRID,
    LAYOUT_VGRID,
    LAYOUT_HGRID,
    LAYOUT_VFLOW,
    LAYOUT_HFLOW,
};

#define LAYOUT_FILL (1 << 0)
#define LAYOUT_GREEDY (1 << 1)
#define LAYOUT_SQUARE (1 << 2)

typedef unsigned LayoutAttributes;

struct Layout
{
    LayoutType type;

    int hcell;
    int vcell;
    Vec2i spacing;
};

#define STACK() \
    ((Layout){LAYOUT_STACK, 0, 0, Vec2i::zero()})

#define GRID(_hcell, _vcell, _hspacing, _vspacing) \
    ((Layout){LAYOUT_GRID, (_hcell), (_vcell), Vec2i((_hspacing), (_vspacing))})

#define VGRID(_vspacing) \
    ((Layout){LAYOUT_VGRID, 0, 0, Vec2i(0, (_vspacing))})

#define HGRID(_hspacing) \
    ((Layout){LAYOUT_HGRID, 0, 0, Vec2i((_hspacing), 0)})

#define VFLOW(_vspacing) \
    ((Layout){LAYOUT_VFLOW, 0, 0, Vec2i(0, (_vspacing))})

#define HFLOW(_hspacing) \
    ((Layout){LAYOUT_HFLOW, 0, 0, Vec2i((_hspacing), 0)})

struct WidgetColor
{
    bool overwritten;
    Color color;
};

class Widget
{
private:
    bool _enabled;
    Rectangle _bound;

    int _max_height = 0;
    int _max_width = 0;
    int _min_height = 0;
    int _min_width = 0;
    Insets _insets = {};
    WidgetColor _colors[__THEME_COLOR_COUNT] = {};
    Layout _layout = {};
    RefPtr<Font> _font;
    LayoutAttributes _layout_attributes = {};

    EventHandler _handlers[EventType::__COUNT] = {};

    struct Widget *_parent = {};
    struct Window *_window = {};

    List *_childs = {};

public:
    void id(String id);

    RefPtr<Font> font()
    {
        if (!_font)
        {
            _font = Font::create("sans").take_value();
        }

        return _font;
    }

    void font(RefPtr<Font> font)
    {
        _font = font;
    }

    Color color(ThemeColorRole role);
    void color(ThemeColorRole role, Color color);

    Rectangle content_bound() const { return bound().shrinked(_insets); }

    Rectangle bound() const { return _bound; }
    void bound(Rectangle value) { _bound = value; }

    Insets insets() const { return _insets; }
    void insets(Insets insets)
    {
        _insets = insets;
        should_relayout();
    }

    void layout(Layout layout) { _layout = layout; }

    void attributes(LayoutAttributes attributes) { _layout_attributes = attributes; }
    LayoutAttributes attributes() { return _layout_attributes; }

    void window(Window *window)
    {
        assert(!_window);

        _window = window;
    }

    Window *window()
    {
        return _window;
    }

    void max_height(int value) { _max_height = value; }

    void max_width(int value) { _max_width = value; }

    void min_height(int value) { _min_height = value; }

    void min_width(int value) { _min_width = value; }

    /* --- subclass API ----------------------------------------------------- */

    Widget(Widget *parent);

    virtual ~Widget();

    virtual void paint(Painter &painter, Rectangle rectangle);

    virtual void event(Event *event);

    virtual void do_layout();

    virtual Vec2i size();

    /* --- Enable/ Disable state -------------------------------------------- */

    bool enabled();

    bool disabled();

    void enable();

    void disable();

    void disable_if(bool condition);

    void enable_if(bool condition);

    /* --- Childs ----------------------------------------------------------- */

    Widget *child_at(Vec2i position);

    void add_child(Widget *child);

    void remove_child(Widget *child);

    void clear_children();

    /* --- Focus state ------------------------------------------------------ */

    bool focused();

    void focus();

    /* --- Paint ------------------------------------------------------------ */

    void repaint(Painter &painter, Rectangle rectangle);

    void should_repaint();

    void should_repaint(Rectangle rectangle);

    /* --- Layout ----------------------------------------------------------- */

    void do_vhgrid_layout(Layout layout, Dimension dim);

    void relayout();

    void should_relayout();

    Vec2i compute_size();

    /* --- Events ----------------------------------------------------------- */

    void on(EventType event, EventHandler handler);

    void dispatch_event(Event *event);
};
