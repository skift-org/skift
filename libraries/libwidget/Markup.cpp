#include <libmarkup/Markup.h>
#include <libsystem/Logger.h>
#include <libsystem/core/CString.h>
#include <libsystem/utils/Lexer.h>
#include <libsystem/utils/NumberParser.h>

#include <libwidget/Markup.h>
#include <libwidget/Widgets.h>

static void whitespace(Lexer &lexer)
{
    lexer.eat(" ");
}

static int number(Lexer &lexer)
{
    int number = 0;

    while (lexer.current_is("0123456789"))
    {
        number *= 10;
        number += lexer.current() - '0';
        lexer.foreward();
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

    Lexer lexer(string, strlen(string));

    if (lexer.skip_word("stack"))
    {
        result = STACK();
    }

    if (lexer.skip_word("grid"))
    {
        lexer.skip('(');

        whitespace(lexer);

        int hcell = number(lexer);

        whitespace(lexer);

        lexer.skip(',');

        whitespace(lexer);

        int vcell = number(lexer);

        whitespace(lexer);

        lexer.skip(',');

        whitespace(lexer);

        int hspacing = number(lexer);

        whitespace(lexer);

        lexer.skip(',');

        whitespace(lexer);

        int vspacing = number(lexer);

        result = GRID(hcell, vcell, hspacing, vspacing);
    }

    if (lexer.skip_word("vgrid"))
    {
        lexer.skip('(');
        whitespace(lexer);
        int spacing = number(lexer);
        result = VGRID(spacing);
    }

    if (lexer.skip_word("hgrid"))
    {
        lexer.skip('(');
        whitespace(lexer);
        int spacing = number(lexer);
        result = HGRID(spacing);
    }

    if (lexer.skip_word("vflow"))
    {
        lexer.skip('(');
        whitespace(lexer);

        int spacing = number(lexer);
        result = VFLOW(spacing);
    }

    if (lexer.skip_word("hflow"))
    {
        lexer.skip('(');
        whitespace(lexer);

        int spacing = number(lexer);
        result = HFLOW(spacing);
    }

    return result;
}

Insets insets_parse(const char *string)
{
    if (!string)
    {
        return Insets(0);
    }

    Lexer lexer(string, strlen(string));

    if (!lexer.skip_word("insets"))
    {
        return Insets(0);
    }

    lexer.skip('(');
    whitespace(lexer);

    int count;
    int args[4];

    for (count = 0; count < 4 && lexer.current_is("0123456789"); count++)
    {
        args[count] = number(lexer);
        whitespace(lexer);
        lexer.skip(',');
        whitespace(lexer);
    }

    Insets result = Insets(0);

    if (count == 1)
    {
        result = Insets(args[0]);
    }
    else if (count == 2)
    {
        result = Insets(args[0], args[1]);
    }
    else if (count == 3)
    {
        result = Insets(args[0], args[1], args[2]);
    }
    else if (count == 4)
    {
        result = Insets(args[0], args[1], args[2], args[3]);
    }

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
    Widget *widget = nullptr;

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
                icon_get(markup_node_get_attribute_or_default(node, "icon", "duck")),
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
                icon_get(markup_node_get_attribute_or_default(node, "icon", "duck")));
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

    if (widget == nullptr)
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

    Window *window = window_create(WINDOW_NONE);

    int width = parse_int_inline(PARSER_DECIMAL, markup_node_get_attribute(node, "width"), 250);

    int height = parse_int_inline(PARSER_DECIMAL, markup_node_get_attribute(node, "height"), 250);

    window_set_size(window, Vec2i(width, height));

    const char *icon = markup_node_get_attribute(node, "icon");

    if (icon)
    {
        window_set_icon(window, icon_get(icon));
    }

    const char *title = markup_node_get_attribute(node, "title");

    if (title)
    {
        window_set_title(window, title);
    }

    return window;
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
