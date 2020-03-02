#include <libgraphic/Painter.h>
#include <libsystem/assert.h>
#include <libsystem/io/Stream.h>
#include <libsystem/logger.h>
#include <libwidget/core/Event.h>
#include <libwidget/core/Widget.h>

void widget_initialize(
    Widget *widget,
    const char *classname,
    Widget *parent,
    Rectangle bound)
{
    widget->classname = classname;
    widget->childs = list_create();
    widget->bound = bound;

    if (parent != NULL)
    {
        widget_add_child(parent, widget);
    }
}

void widget_destroy(Widget *widget)
{
    logger_info("Destroying %s(%08x)", widget->classname, widget);

    if (widget->destroy)
    {
        widget->destroy(widget);
    }

    Widget *child = NULL;
    while (list_peek(widget->childs, (void **)&child))
    {
        widget_remove_child(widget, child);
        widget_destroy(child);
    }

    list_destroy(widget->childs);

    free(widget);
}

void widget_add_child(Widget *widget, Widget *child)
{
    assert(child->parent == NULL);

    logger_info("Adding child %s(0x%08x) to %s(0x%08x)", child->classname, child, widget->classname, widget);

    child->parent = widget;
    list_pushback(widget->childs, child);

    widget_raise(widget, &(Event){widget, EVENT_CHILD_ADDED, false});
}

void widget_remove_child(Widget *widget, Widget *child)
{
    assert(child->parent == widget);

    logger_info("Removing child %s(0x%08x) from %s(0x%08x)", child->classname, child, widget->classname, widget);

    child->parent = NULL;
    list_remove(widget->childs, child);

    widget_raise(widget, &(Event){widget, EVENT_CHILD_REMOVED, false});
}

void widget_raise(Widget *widget, Event *event)
{
    widget_event(widget, event);

    if (!event->accepted && widget->parent)
    {
        widget_raise(widget->parent, event);
    }
}

void widget_event(Widget *widget, Event *event)
{
    if (event->type == EVENT_CHILD_ADDED ||
        event->type == EVENT_CHILD_REMOVED)
    {
        event->accepted = true;

        widget_raise(widget, &(Event){widget, EVENT_PAINT, false});
    }

    if (widget->event)
    {
        widget->event(widget, event);
    }
}

void widget_paint(Widget *widget, Painter *painter)
{
    if (widget->paint)
    {
        widget->paint(widget, painter);
    }

    painter_push_origin(painter, widget->bound.position);

    list_foreach(Widget, child, widget->childs)
    {
        widget_paint(child, painter);
    }

    painter_pop_origin(painter);
}

void widget_dump_iternal(Widget *widget, int depth)
{
    for (int i = 0; i < depth; i++)
    {
        printf("\t");
    }

    printf("%s(0x%08x) (%d, %d) %dx%d\n",
           widget->classname,
           widget,
           widget->bound.X,
           widget->bound.Y,
           widget->bound.width,
           widget->bound.height);

    list_foreach(Widget, child, widget->childs)
    {
        widget_dump_iternal(child, depth + 1);
    }
}

void widget_dump(Widget *widget)
{
    widget_dump_iternal(widget, 0);
}
