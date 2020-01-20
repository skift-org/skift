#pragma once

#include <libgraphic/shape.h>
#include <libsystem/list.h>

struct Widget;

typedef void (*WidgetDestroyCallback)(struct Widget *widget);

typedef struct Widget
{
    const char *classname;
    WidgetDestroyCallback destroy;

    Rectangle bound;
    struct Widget *parent;
    List *childs;
} Widget;

#define WIDGET(__subclass) ((Widget *)(__subclass))

void widget_initialize(
    Widget *widget,
    const char *classname,
    Widget *parent,
    Rectangle bound);

void widget_destroy(Widget *widget);

void widget_add_child(Widget *widget, Widget *child);

void widget_remove_child(Widget *widget, Widget *child);

void widget_dump(Widget *widget);
