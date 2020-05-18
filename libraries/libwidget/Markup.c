#include <libmarkup/Markup.h>
#include <libsystem/utils/NumberParser.h>
#include <libwidget/Markup.h>

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

    markup_node_destroy(root);

    return window;
}
