#include <libmarkup/Markup.h>
#include <libsystem/cstring.h>
#include <libsystem/logger.h>
#include <libsystem/utils/NumberParser.h>
#include <libsystem/utils/SourceReader.h>

#include <libwidget/Markup.h>
#include <libwidget/Widgets.h>

static void whitespace(SourceReader *reader)
{
    source_eat(reader, " ");
}

static int number(SourceReader *reader)
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

        whitespace(reader);

        int hcell = number(reader);

        whitespace(reader);

        source_skip(reader, ',');

        whitespace(reader);

        int vcell = number(reader);

        whitespace(reader);

        source_skip(reader, ',');

        whitespace(reader);

        int hspacing = number(reader);

        whitespace(reader);

        source_skip(reader, ',');

        whitespace(reader);

        int vspacing = number(reader);

        result = GRID(hcell, vcell, hspacing, vspacing);
    }

    if (source_skip_word(reader, "vgrid"))
    {
        source_skip(reader, '(');
        whitespace(reader);
        int spacing = number(reader);
        result = VGRID(spacing);
    }

    if (source_skip_word(reader, "hgrid"))
    {
        source_skip(reader, '(');
        whitespace(reader);
        int spacing = number(reader);
        result = HGRID(spacing);
    }

    if (source_skip_word(reader, "vflow"))
    {
        source_skip(reader, '(');
        whitespace(reader);

        int spacing = number(reader);
        result = VFLOW(spacing);
    }

    if (source_skip_word(reader, "hflow"))
    {
        source_skip(reader, '(');
        whitespace(reader);

        int spacing = number(reader);
        result = HFLOW(spacing);
    }

    source_destroy(reader);

    return result;
}

Insets insets_parse(const char *string)
{
    if (!string)
    {
        return INSETS(0);
    }

    SourceReader *reader = source_create_from_string(string, strlen(string));

    if (!source_skip_word(reader, "insets"))
    {
        source_destroy(reader);

        return INSETS(0);
    }

    source_skip(reader, '(');
    whitespace(reader);

    int count;
    int args[4];

    for (count = 0; count < 4 && source_current_is(reader, "0123456789"); count++)
    {
        args[count] = number(reader);
        whitespace(reader);
        source_skip(reader, ',');
        whitespace(reader);
    }

    Insets result = INSETS(0);

    if (count == 1)
    {
        result = INSETS(args[0]);
    }
    else if (count == 2)
    {
        result = INSETS(args[0], args[1]);
    }
    else if (count == 3)
    {
        result = INSETS(args[0], args[1], args[2]);
    }
    else if (count == 4)
    {
        result = INSETS(args[0], args[1], args[2], args[3]);
    }

    source_destroy(reader);

    return result;
}

void widget_apply_attribute_from_markup(Widget *widget, MarkupNode *node)
{
    if (markup_node_has_attribute(node, "id"))
    {
        window_register_widget_by_id(
            widget->window,
            markup_node_get_attribute(node, "id"),
            widget);
    }

    if (markup_node_has_attribute(node, "layout"))
    {
        widget->layout = layout_parse(markup_node_get_attribute(node, "layout"));
    }

    if (markup_node_has_attribute(node, "padding"))
    {
        widget->insets = insets_parse(markup_node_get_attribute(node, "padding"));
    }

    if (markup_node_has_attribute(node, "fill"))
    {
        widget->layout_attributes = LAYOUT_FILL;
    }
}

Widget *widget_create_from_markup(Widget *parent, MarkupNode *node)
{
    Widget *widget = NULL;

    if (markup_node_is(node, "Container"))
    {
        widget = container_create(parent);
    }

    if (markup_node_is(node, "Panel"))
    {
        widget = panel_create(parent);
    }

    if (markup_node_is(node, "Button"))
    {
        ButtonStyle button_style = BUTTON_TEXT;

        if (markup_node_has_attribute(node, "filled"))
        {
            button_style = BUTTON_FILLED;
        }

        if (markup_node_has_attribute(node, "outlined"))
        {
            button_style = BUTTON_OUTLINE;
        }

        if (markup_node_has_attribute(node, "text") && markup_node_has_attribute(node, "icon"))
        {
            widget = button_create_with_icon_and_text(
                parent,
                button_style,
                markup_node_get_attribute_or_default(node, "icon", "duck"),
                markup_node_get_attribute_or_default(node, "text", "Button"));
        }
        else if (markup_node_has_attribute(node, "text"))
        {
            widget = button_create_with_text(
                parent,
                button_style,
                markup_node_get_attribute_or_default(node, "text", "Button"));
        }
        else if (markup_node_has_attribute(node, "icon"))
        {
            widget = button_create_with_icon(
                parent,
                button_style,
                markup_node_get_attribute_or_default(node, "icon", "duck"));
        }
        else
        {
            widget = button_create(
                parent,
                button_style);
        }
    }

    if (markup_node_is(node, "Label"))
    {
        widget = label_create(
            parent,
            markup_node_get_attribute_or_default(node, "text", "Label"));
    }

    if (markup_node_is(node, "Image"))
    {
        widget = image_create(
            parent,
            markup_node_get_attribute_or_default(node, "path", "null"));
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
