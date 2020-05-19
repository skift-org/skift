#pragma once

#include <libgraphic/Font.h>
#include <libgraphic/Shape.h>
#include <libsystem/utils/List.h>
#include <libwidget/Event.h>
#include <libwidget/Theme.h>

struct Widget;
struct Event;
struct Painter;
struct Window;

typedef Vec2i (*WidgetComputeSizeCallback)(struct Widget *widget);
typedef void (*WidgetDestroyCallback)(struct Widget *widget);
typedef void (*WidgetPaintCallback)(struct Widget *widget, struct Painter *painter, Rectangle rectangle);
typedef void (*WidgetEventCallback)(struct Widget *widget, struct Event *event);
typedef void (*WidgetLayoutCallback)(struct Widget *widget);

typedef void (*WidgetEventHandlerCallback)(void *target, struct Widget *sender, struct Event *event);

typedef struct
{
    void *target;
    WidgetEventHandlerCallback callback;
} WidgetEventHandler;

typedef enum
{
    LAYOUT_STACK,
    LAYOUT_GRID,
    LAYOUT_VGRID,
    LAYOUT_HGRID,
    LAYOUT_VFLOW,
    LAYOUT_HFLOW,
} LayoutType;

typedef enum
{
    LAYOUT_FILL = 1 << 0,
} LayoutAttributes;

typedef enum
{
    CONTENT_LEFT,
    CONTENT_CENTER,
    CONTENT_RIGHT,
} ContentAlign;

typedef struct
{
    LayoutType type;

    int hcell;
    int vcell;
    Vec2i spacing;
} Layout;

#define STACK() \
    ((Layout){LAYOUT_STACK, 0, 0, vec2i_zero})

#define GRID(_hcell, _vcell, _hspacing, _vspacing) \
    ((Layout){LAYOUT_GRID, (_hcell), (_vcell), vec2i((_hspacing), (_vspacing))})

#define VGRID(_vspacing) \
    ((Layout){LAYOUT_VGRID, 0, 0, vec2i(0, (_vspacing))})

#define HGRID(_hspacing) \
    ((Layout){LAYOUT_HGRID, 0, 0, vec2i((_hspacing), 0)})

#define VFLOW(_vspacing) \
    ((Layout){LAYOUT_VFLOW, 0, 0, vec2i(0, (_vspacing))})

#define HFLOW(_hspacing) \
    ((Layout){LAYOUT_HFLOW, 0, 0, vec2i((_hspacing), 0)})

typedef struct
{
    bool overwritten;
    Color color;
} WidgetColorOverwrite;

typedef struct Widget
{
    const char *classname;

    bool enabled;
    Rectangle bound;
    Insets insets;
    WidgetColorOverwrite color_overwrite[__THEME_COLOR_COUNT];

    WidgetDestroyCallback destroy;
    WidgetPaintCallback paint;
    WidgetEventCallback event;
    WidgetComputeSizeCallback size;
    WidgetLayoutCallback do_layout;

    WidgetEventHandler event_handles[__EVENT_TYPE_COUNT];

    struct Widget *parent;
    struct Window *window;
    Layout layout; // FIXME: this shoul be a separeted object
    LayoutAttributes layout_attributes;
    List *childs;
} Widget;

#define WIDGET(__subclass) ((Widget *)(__subclass))

Font *widget_font(void);

void widget_initialize(
    Widget *widget,
    const char *classname,
    Widget *parent);

void widget_destroy(Widget *widget);

void widget_add_child(Widget *widget, Widget *child);

void widget_remove_child(Widget *widget, Widget *child);

void widget_dump(Widget *widget, int depth);

void widget_dispatch_event(Widget *widget, struct Event *event);

void widget_paint(Widget *widget, struct Painter *painter, Rectangle rectangle);

void widget_layout(Widget *widget);

void widget_focus(Widget *widget);

void widget_set_event_handler(Widget *widget, EventType event, void *target, WidgetEventHandlerCallback callback);

void widget_clear_event_handler(Widget *widget, EventType event);

Vec2i widget_compute_size(Widget *widget);

Rectangle __widget_bound(Widget *widget);
#define widget_bound(__widget) __widget_bound(WIDGET(__widget))

Rectangle __widget_content_bound(Widget *widget);
#define widget_content_bound(__widget) __widget_content_bound(WIDGET(__widget))

void widget_update(Widget *widget);
void widget_update_region(Widget *widget, Rectangle region);

Widget *widget_child_at(Widget *parent, Vec2i position);

Color __widget_get_color(Widget *widget, ThemeColorRole role);

#define widget_get_color(__widget, __role) __widget_get_color(WIDGET(__widget), (__role))

void __widget_overwrite_color(Widget *widget, ThemeColorRole role, Color color);
#define widget_overwrite_color(__widget, __role, __color) __widget_overwrite_color(WIDGET(__widget), (__role), (__color))

void widget_set_enable(Widget *widget, bool enable);

bool widget_is_enable(Widget *widget);
