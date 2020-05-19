#include <libmarkup/Markup.h>
#include <libsystem/cstring.h>
#include <libsystem/utils/NumberParser.h>
#include <libwidget/Markup.h>

#include <libwidget/Container.h>
#include <libwidget/Label.h>

void widget_apply_attribute_from_markup(Widget *widget, MarkupNode *node)
{
    __unused(widget);
    __unused(node);

    widget->layout = VGRID(0);
}

Widget *widget_create_from_markup(Widget *parent, MarkupNode *node)
{
    Widget *widget = NULL;

    if (markup_node_is(node, "Container"))
    {
        widget = container_create(parent);
    }

    if (widget == NULL)
    {
        char buffer[256];
        snprintf(buffer, 256, "Unknown widget %s", markup_node_type(node));
        widget = label_create(parent, buffer);
    }

    widget_apply_attribute_from_markup(widget, node);

    return widget;
}

void widget_create_childs_from_markup(Widget *parent, MarkupNode *node)
{
    list_foreach(MarkupNode, child, node->childs)
    {
        Widget *child_widget = widget_create_from_markup(parent, child);

        widget_create_childs_from_markup(child_widget, child);
    }
}

Window *window_create_from_markup(MarkupNode *node)
{
    const char *icon = markup_node_get_attribute_or_default(node, "icon", "application");
    const char *title = markup_node_get_attribute_or_default(node, "title", "Window");

    int width = parse_int_inline(PARSER_DECIMAL, markup_node_get_attribute(node, "width"), 250);
    int height = parse_int_inline(PARSER_DECIMAL, markup_node_get_attribute(node, "height"), 250);

    return window_create(icon, title, width, height, WINDOW_NONE);
}

Window *window_create_from_file(const char *path)
{
    MarkupNode *root = markup_parse_file(path);

    Window *window = window_create_from_markup(root);
    widget_apply_attribute_from_markup(window_root(window), root);
    widget_create_childs_from_markup(window_root(window), root);

    markup_node_destroy(root);

    return window;
}
