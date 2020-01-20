#pragma once

#include <libsystem/list.h>

struct Widget;

typedef void (*WidgetDestroyCallback)(struct Widget *widget);

typedef struct Widget
{
    const char *classname;

    struct Widget *parent;
    List *childs;

    WidgetDestroyCallback destroy;
} Widget;

#define WIDGET(__subclass) ((Widget *)(__subclass))

void widget_initialize(const char *classname, Widget *widget, Widget *parent);

void widget_destroy(Widget *widget);

void widget_add_child(Widget *widget, Widget *child);

void widget_remove_child(Widget *widget, Widget *child);

void widget_dump(Widget *widget);
