#include <libmarkup/Markup.h>
#include <libsystem/Assert.h>
#include <libsystem/CString.h>
#include <libsystem/Logger.h>
#include <libsystem/unicode/Codepoint.h>
#include <libsystem/utils/BufferBuilder.h>
#include <libsystem/utils/NumberParser.h>
#include <libsystem/utils/SourceReader.h>

#define MARKUP_WHITESPACE " \n\r\t"
#define MARKUP_XDIGITS "0123456789abcdef"
#define MARKUP_ALPHA "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"

static void whitespace(SourceReader *source)
{
    source_eat(source, MARKUP_WHITESPACE);
}

static char *identifier(SourceReader *source)
{
    BufferBuilder *builder = buffer_builder_create(6);

    while (source_current_is(source, MARKUP_ALPHA) &&
           source_do_continue(source))
    {
        buffer_builder_append_chr(builder, source_current(source));
        source_foreward(source);
    }

    return buffer_builder_finalize(builder);
}

char *string(SourceReader *source)
{
    BufferBuilder *builder = buffer_builder_create(16);

    source_skip(source, '"');

    while (source_current(source) != '"' &&
           source_do_continue(source))
    {
        if (source_current(source) == '\\')
        {
            buffer_builder_append_str(builder, source_read_escape_sequence(source));
        }
        else
        {
            buffer_builder_append_chr(builder, source_current(source));
            source_foreward(source);
        }
    }

    source_skip(source, '"');

    return buffer_builder_finalize(builder);
}

static MarkupAttribute *attribute(SourceReader *source)
{
    char *ident = identifier(source);

    whitespace(source);

    if (source_skip(source, '='))
    {

        whitespace(source);

        return markup_attribute_create_adopt(ident, string(source));
    }
    else
    {
        return markup_attribute_create_adopt(ident, NULL);
    }
}

static bool opening_tag(MarkupNode **node, SourceReader *source)
{
    source_skip(source, '<');

    whitespace(source);

    *node = markup_node_create_adopt(identifier(source));

    whitespace(source);

    while (source_current_is(source, MARKUP_ALPHA) &&
           source_do_continue(source))
    {
        markup_node_add_attribute(*node, attribute(source));
        whitespace(source);
    }

    if (source_skip(source, '/'))
    {
        source_skip(source, '>');

        return false;
    }

    source_skip(source, '>');

    return true;
}

static void closing_tag(MarkupNode *node, SourceReader *source)
{
    source_skip(source, '<');
    source_skip(source, '/');

    whitespace(source);

    char *node_other_name = identifier(source);

    if (strcmp(markup_node_type(node), node_other_name) != 0)
    {
        logger_warn(
            "Opening tag <%s> doesn't match closing tag </%s>",
            markup_node_type(node),
            node_other_name);
    }

    free(node_other_name);

    whitespace(source);

    source_skip(source, '>');
}

static MarkupNode *parse_node(SourceReader *source)
{
    whitespace(source);

    MarkupNode *node = NULL;
    if (!opening_tag(&node, source))
    {
        return node;
    }

    whitespace(source);

    while (source_peek(source, 0) == '<' &&
           source_peek(source, 1) != '/' &&
           source_do_continue(source))
    {
        markup_node_add_child(node, parse_node(source));
        whitespace(source);
    }

    whitespace(source);

    closing_tag(node, source);

    return node;
}

MarkupNode *markup_parse(const char *string, size_t size)
{
    SourceReader *source = source_create_from_string(string, size);

    MarkupNode *node = parse_node(source);

    source_destroy(source);

    return node;
}

MarkupNode *markup_parse_file(const char *path)
{
    __cleanup(stream_cleanup) Stream *json_file = stream_open(path, OPEN_READ);

    if (handle_has_error(json_file))
    {
        return NULL;
    }

    SourceReader *source = source_create_from_stream(json_file);

    MarkupNode *node = parse_node(source);

    source_destroy(source);

    return node;
}
