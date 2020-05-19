#include <libmarkup/Markup.h>
#include <libsystem/cstring.h>
#include <libsystem/logger.h>
#include <libsystem/utils/NumberParser.h>
#include <libsystem/utils/SourceReader.h>

#include <libwidget/Button.h>
#include <libwidget/Container.h>
#include <libwidget/Label.h>
#include <libwidget/Markup.h>
#include <libwidget/Placeholder.h>

static int layout_number(SourceReader *reader)
{
    int number = 0;

    while (source_current_is(reader, "0123456789"))
    {
        number *= 10;
        number += source_current(reader) - '0';
        source_foreward(reader);
    }

    return number;
}

static Layout layout_parse(const char *string)
{
    if (!string)
    {
        return STACK();
    }

    Layout result = STACK();

    SourceReader *reader = source_create_from_string(string, strlen(string));

    if (source_skip_word(reader, "stack"))
    {
        result = STACK();
    }

    if (source_skip_word(reader, "grid"))
    {
        source_skip(reader, '(');
        int hcell = layout_number(reader);
        source_skip(reader, ',');
        int vcell = layout_number(reader);
        source_skip(reader, ',');
        int hspacing = layout_number(reader);
        source_skip(reader, ',');
        int vspacing = layout_number(reader);

        result = GRID(hcell, vcell, hspacing, vspacing);
    }

    if (source_skip_word(reader, "vgrid"))
    {
        source_skip(reader, '(');
        int spacing = layout_number(reader);
        result = VGRID(spacing);
    }

    if (source_skip_word(reader, "hgrid"))
    {
        source_skip(reader, '(');
        int spacing = layout_number(reader);
        result = HGRID(spacing);
    }

    if (source_skip_word(reader, "vflow"))
    {
        source_skip(reader, '(');
        int spacing = layout_number(reader);
        result = VFLOW(spacing);
    }

    if (source_skip_word(reader, "hflow"))
    {
        source_skip(reader, '(');
        int spacing = layout_number(reader);
        result = HFLOW(spacing);
    }

    source_destroy(reader);

    return result;
}

void widget_apply_attribute_from_markup(Widget *widget, MarkupNode *node)
{
    if (markup_node_has_attribute(node, "layout"))
    {
        widget->layout = layout_parse(markup_node_get_attribute(node, "layout"));
    }
}

Widget *widget_create_from_markup(Widget *parent, MarkupNode *node)
{
    Widget *widget = NULL;

    if (markup_node_is(node, "Container"))
    {
        widget = container_create(parent);
    }

    if (markup_node_is(node, "Button"))
    {
        widget = button_create(
            parent,
            markup_node_get_attribute_or_default(node, "text", "Button"));
    }

    if (markup_node_is(node, "Label"))
    {
        widget = label_create(
            parent,
            markup_node_get_attribute_or_default(node, "text", "Label"));
    }

    if (widget == NULL)
    {
        widget = placeholder_create(parent, markup_node_type(node));
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

    logger_trace("Window size: %s %d", markup_node_get_attribute(node, "width"), width);

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
