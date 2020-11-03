#include <libmarkup/Markup.h>
#include <libsystem/Assert.h>
#include <libsystem/Logger.h>
#include <libsystem/core/CString.h>
#include <libsystem/unicode/Codepoint.h>
#include <libsystem/utils/BufferBuilder.h>
#include <libsystem/utils/NumberParser.h>

#include <libutils/Scanner.h>
#include <libutils/ScannerUtils.h>

#define MARKUP_WHITESPACE " \n\r\t"
#define MARKUP_XDIGITS "0123456789abcdef"
#define MARKUP_ALPHA "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"

static void whitespace(Scanner &scan)
{
    scan.eat(MARKUP_WHITESPACE);
}

static char *identifier(Scanner &scan)
{
    BufferBuilder *builder = buffer_builder_create(6);

    while (scan.current_is(MARKUP_ALPHA) &&
           scan.do_continue())
    {
        buffer_builder_append_chr(builder, scan.current());
        scan.foreward();
    }

    return buffer_builder_finalize(builder);
}

char *string(Scanner &scan)
{
    BufferBuilder *builder = buffer_builder_create(16);

    scan.skip('"');

    while (scan.current() != '"' &&
           scan.do_continue())
    {
        if (scan.current() == '\\')
        {
            buffer_builder_append_str(builder, scan_json_escape_sequence(scan));
        }
        else
        {
            buffer_builder_append_chr(builder, scan.current());
            scan.foreward();
        }
    }

    scan.skip('"');

    return buffer_builder_finalize(builder);
}

static MarkupAttribute *attribute(Scanner &scan)
{
    char *ident = identifier(scan);

    whitespace(scan);

    if (scan.skip('='))
    {

        whitespace(scan);

        return markup_attribute_create_adopt(ident, string(scan));
    }
    else
    {
        return markup_attribute_create_adopt(ident, nullptr);
    }
}

static bool opening_tag(MarkupNode **node, Scanner &scan)
{
    scan.skip('<');

    whitespace(scan);

    *node = markup_node_create_adopt(identifier(scan));

    whitespace(scan);

    while (scan.current_is(MARKUP_ALPHA) &&
           scan.do_continue())
    {
        markup_node_add_attribute(*node, attribute(scan));
        whitespace(scan);
    }

    if (scan.skip('/'))
    {
        scan.skip('>');

        return false;
    }

    scan.skip('>');

    return true;
}

static void closing_tag(MarkupNode *node, Scanner &scan)
{
    scan.skip('<');
    scan.skip('/');

    whitespace(scan);

    char *node_other_name = identifier(scan);

    if (strcmp(markup_node_type(node), node_other_name) != 0)
    {
        logger_warn(
            "Opening tag <%s> doesn't match closing tag </%s>",
            markup_node_type(node),
            node_other_name);
    }

    free(node_other_name);

    whitespace(scan);

    scan.skip('>');
}

static MarkupNode *node(Scanner &scan)
{
    whitespace(scan);

    MarkupNode *current_node = nullptr;
    if (!opening_tag(&current_node, scan))
    {
        return current_node;
    }

    whitespace(scan);

    while (scan.peek(0) == '<' &&
           scan.peek(1) != '/' &&
           scan.do_continue())
    {
        markup_node_add_child(current_node, node(scan));
        whitespace(scan);
    }

    whitespace(scan);

    closing_tag(current_node, scan);

    return current_node;
}

MarkupNode *markup_parse(const char *string, size_t size)
{
    StringScanner scan{string, size};
    scan_skip_utf8bom(scan);
    return node(scan);
}

MarkupNode *markup_parse_file(const char *path)
{
    __cleanup(stream_cleanup) Stream *markup_file = stream_open(path, OPEN_READ);

    if (handle_has_error(markup_file))
    {
        return nullptr;
    }

    StreamScanner scan{markup_file};
    scan_skip_utf8bom(scan);
    return node(scan);
}
