#include <libsystem/assert.h>
#include <libsystem/logger.h>
#include <libwidget/core/Widget.h>

void widget_initialize(const char *classname, Widget *widget, Widget *parent)
{
    widget->classname = classname;
    widget->childs = list_create();

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

    list_destroy(widget->childs, LIST_KEEP_VALUES);

    free(widget);
}

void widget_add_child(Widget *widget, Widget *child)
{
    assert(child->parent == NULL);

    logger_info("Adding child %s(0x%08x)", child->classname, child);

    child->parent = widget;
    list_pushback(widget->childs, child);
}

void widget_remove_child(Widget *widget, Widget *child)
{
    assert(child->parent == widget);

    logger_info("Removing child %s(0x%08x)", child->classname, child);

    child->parent = NULL;
    list_remove(widget->childs, child);
}

void widget_dump_iternal(Widget *widget, int depth)
{
    for (int i = 0; i < depth; i++)
    {
        printf("\t");
    }

    printf("%s(0x%08x)\n", widget->classname, widget);

    list_foreach(Widget, child, widget->childs)
    {
        widget_dump_iternal(child, depth + 1);
    }
}

void widget_dump(Widget *widget)
{
    widget_dump_iternal(widget, 0);
}
