#pragma once

#include <libgraphic/Font.h>
#include <libgraphic/Shape.h>
#include <libsystem/utils/List.h>

struct Widget;
struct Event;
struct Painter;
struct Window;

typedef void (*WidgetDestroyCallback)(struct Widget *widget);
typedef void (*WidgetPaintCallback)(struct Widget *widget, struct Painter *painter);
typedef void (*WidgetEventCallback)(struct Widget *widget, struct Event *event);

typedef enum
{
    LAYOUT_STACK,
    LAYOUT_VFLOW,
    LAYOUT_HFLOW,
} LayoutType;

typedef struct
{
    LayoutType type;

    int hspacing;
    int vspacing;
} Layout;

typedef struct Widget
{
    const char *classname;

    Rectangle bound;
    Insets insets;

    WidgetDestroyCallback destroy;
    WidgetPaintCallback paint;
    WidgetEventCallback event;

    struct Widget *parent;
    struct Window *window;
    Layout layout; // FIXME: this shoul be a separeted object
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

void widget_handle_event(Widget *widget, struct Event *event);

void widget_paint(Widget *widget, struct Painter *painter);

void widget_layout(Widget *widget);

void widget_focus(Widget *widget);

Rectangle __widget_bound(Widget *widget);
#define widget_bound(__widget) __widget_bound(WIDGET(__widget))

Rectangle __widget_content_bound(Widget *widget);
#define widget_content_bound(__widget) __widget_content_bound(WIDGET(__widget))

void widget_update(Widget *widget);
