#pragma once

#include <libgraphic/Font.h>
#include <libgraphic/Shape.h>
#include <libsystem/utils/List.h>
#include <libwidget/Event.h>
#include <libwidget/Theme.h>

struct Widget;
struct Painter;
struct Window;

typedef Vec2i (*WidgetComputeSizeCallback)(struct Widget *widget);
typedef void (*WidgetDestroyCallback)(struct Widget *widget);
typedef void (*WidgetPaintCallback)(struct Widget *widget, struct Painter &painter, Rectangle rectangle);
typedef void (*WidgetEventCallback)(struct Widget *widget, struct Event *event);
typedef void (*WidgetLayoutCallback)(struct Widget *widget);

typedef enum
{
    LAYOUT_STACK,
    LAYOUT_GRID,
    LAYOUT_VGRID,
    LAYOUT_HGRID,
    LAYOUT_VFLOW,
    LAYOUT_HFLOW,
} LayoutType;

#define LAYOUT_FILL (1 << 0)
#define LAYOUT_GREEDY (1 << 1)

typedef unsigned LayoutAttributes;

typedef struct
{
    LayoutType type;

    int hcell;
    int vcell;
    Vec2i spacing;
} Layout;

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

typedef struct
{
    bool overwritten;
    Color color;
} WidgetColor;

typedef struct WidgetClass
{
    const char *name;

    WidgetDestroyCallback destroy = nullptr;
    WidgetPaintCallback paint = nullptr;
    WidgetEventCallback event = nullptr;
    WidgetComputeSizeCallback size = nullptr;
    WidgetLayoutCallback layout = nullptr;
} WidgetClass;

typedef struct Widget
{
    const WidgetClass *klass;

    bool enabled;
    Rectangle bound;

    int max_height;
    int max_width;
    int min_height;
    int min_width;
    Insets insets;
    WidgetColor colors[__THEME_COLOR_COUNT];
    Layout layout; // FIXME: this shoul be a separeted object
    LayoutAttributes layout_attributes;

    EventHandler handlers[__EVENT_TYPE_COUNT];

    struct Widget *parent;
    struct Window *window;
    List *childs;
} Widget;

#define WIDGET(__subclass) ((Widget *)(__subclass))

RefPtr<Font> widget_font();

void widget_initialize(
    Widget *widget,
    const WidgetClass *klass,
    Widget *parent);

void widget_destroy(Widget *widget);

void widget_dump(Widget *widget, int depth);

void widget_event(Widget *widget, struct Event *event);

void widget_paint(Widget *widget, struct Painter &painter, Rectangle rectangle);

void widget_layout(Widget *widget);

void widget_update(Widget *widget);

void widget_update_region(Widget *widget, Rectangle region);

void widget_set_event_handler(Widget *widget, EventType event, EventHandler handler);

void widget_clear_event_handler(Widget *widget, EventType event);

/* --- Widget childs ------------------------------------ */

Widget *widget_get_child_at(Widget *parent, Vec2i position);

void widget_add_child(Widget *widget, Widget *child);

void widget_remove_child(Widget *widget, Widget *child);

void widget_clear_childs(Widget *widget);

/* --- Widget enable state ------------------------------ */

void widget_set_focus(Widget *widget);

bool widget_is_enable(Widget *widget);

void widget_set_enable(Widget *widget, bool enable);

void widget_disable(Widget *widget);

void widget_enable(Widget *widget);

/* --- Widget Style ------------------------------------- */

Color __widget_get_color(Widget *widget, ThemeColorRole role);
#define widget_get_color(__widget, __role) __widget_get_color(WIDGET(__widget), (__role))

void __widget_set_color(Widget *widget, ThemeColorRole role, Color color);
#define widget_set_color(__widget, __role, __color) __widget_set_color(WIDGET(__widget), (__role), (__color))

Rectangle __widget_get_bound(Widget *widget);
#define widget_get_bound(__widget) __widget_get_bound(WIDGET(__widget))

Rectangle __widget_get_content_bound(Widget *widget);
#define widget_get_content_bound(__widget) __widget_get_content_bound(WIDGET(__widget))

Vec2i widget_compute_size(Widget *widget);
