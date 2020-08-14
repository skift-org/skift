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

struct Widget
{
    bool _enabled;
    Rectangle bound;

    int max_height = 0;
    int max_width = 0;
    int min_height = 0;
    int min_width = 0;
    Insets insets = {};
    WidgetColor colors[__THEME_COLOR_COUNT] = {};
    Layout layout = {}; // FIXME: this shoul be a separeted object
    LayoutAttributes layout_attributes = {};

    EventHandler handlers[EventType::__COUNT] = {};

    struct Widget *parent = {};
    struct Window *window = {};
    List *childs = {};

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

    /* --- Focus state ------------------------------------------------------ */

    bool focused();

    void focus();

    /* --- Paint ------------------------------------------------------------ */

    void should_repaint();

    void should_repaint(Rectangle rectangle);

    /* --- Layout ----------------------------------------------------------- */

    void relayout();

    void should_relayout();

    /* --- Events ----------------------------------------------------------- */

    void on(EventType event, EventHandler handler);
};

RefPtr<Font> widget_font();

void widget_event(Widget *widget, struct Event *event);

void widget_paint(Widget *widget, struct Painter &painter, Rectangle rectangle);

/* --- Widget childs ------------------------------------ */

Widget *widget_get_child_at(Widget *parent, Vec2i position);

void widget_add_child(Widget *widget, Widget *child);

void widget_remove_child(Widget *widget, Widget *child);

void widget_clear_childs(Widget *widget);

/* --- Widget Style ------------------------------------- */

Color widget_get_color(Widget *widget, ThemeColorRole role);

void widget_set_color(Widget *widget, ThemeColorRole role, Color color);

Rectangle widget_get_bound(const Widget *widget);

Rectangle widget_get_content_bound(const Widget *widget);

Vec2i widget_compute_size(Widget *widget);
