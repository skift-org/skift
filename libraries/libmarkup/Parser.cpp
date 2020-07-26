#include <libmarkup/Markup.h>
#include <libsystem/Assert.h>
#include <libsystem/Logger.h>
#include <libsystem/core/CString.h>
#include <libsystem/unicode/Codepoint.h>
#include <libsystem/utils/BufferBuilder.h>
#include <libsystem/utils/Lexer.h>
#include <libsystem/utils/NumberParser.h>

#define MARKUP_WHITESPACE " \n\r\t"
#define MARKUP_XDIGITS "0123456789abcdef"
#define MARKUP_ALPHA "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"

static void whitespace(Lexer &lexer)
{
    lexer.eat(MARKUP_WHITESPACE);
}

static char *identifier(Lexer &lexer)
{
    BufferBuilder *builder = buffer_builder_create(6);

    while (lexer.current_is(MARKUP_ALPHA) &&
           lexer.do_continue())
    {
        buffer_builder_append_chr(builder, lexer.current());
        lexer.foreward();
    }

    return buffer_builder_finalize(builder);
}

char *string(Lexer &lexer)
{
    BufferBuilder *builder = buffer_builder_create(16);

    lexer.skip('"');

    while (lexer.current() != '"' &&
           lexer.do_continue())
    {
        if (lexer.current() == '\\')
        {
            buffer_builder_append_str(builder, lexer.read_escape_sequence());
        }
        else
        {
            buffer_builder_append_chr(builder, lexer.current());
            lexer.foreward();
        }
    }

    lexer.skip('"');

    return buffer_builder_finalize(builder);
}

static MarkupAttribute *attribute(Lexer &lexer)
{
    char *ident = identifier(lexer);

    whitespace(lexer);

    if (lexer.skip('='))
    {

        whitespace(lexer);

        return markup_attribute_create_adopt(ident, string(lexer));
    }
    else
    {
        return markup_attribute_create_adopt(ident, nullptr);
    }
}

static bool opening_tag(MarkupNode **node, Lexer &lexer)
{
    lexer.skip('<');

    whitespace(lexer);

    *node = markup_node_create_adopt(identifier(lexer));

    whitespace(lexer);

    while (lexer.current_is(MARKUP_ALPHA) &&
           lexer.do_continue())
    {
        markup_node_add_attribute(*node, attribute(lexer));
        whitespace(lexer);
    }

    if (lexer.skip('/'))
    {
        lexer.skip('>');

        return false;
    }

    lexer.skip('>');

    return true;
}

static void closing_tag(MarkupNode *node, Lexer &lexer)
{
    lexer.skip('<');
    lexer.skip('/');

    whitespace(lexer);

    char *node_other_name = identifier(lexer);

    if (strcmp(markup_node_type(node), node_other_name) != 0)
    {
        logger_warn(
            "Opening tag <%s> doesn't match closing tag </%s>",
            markup_node_type(node),
            node_other_name);
    }

    free(node_other_name);

    whitespace(lexer);

    lexer.skip('>');
}

static MarkupNode *node(Lexer &lexer)
{
    whitespace(lexer);

    MarkupNode *current_node = nullptr;
    if (!opening_tag(&current_node, lexer))
    {
        return current_node;
    }

    whitespace(lexer);

    while (lexer.peek(0) == '<' &&
           lexer.peek(1) != '/' &&
           lexer.do_continue())
    {
        markup_node_add_child(current_node, node(lexer));
        whitespace(lexer);
    }

    whitespace(lexer);

    closing_tag(current_node, lexer);

    return current_node;
}

MarkupNode *markup_parse(const char *string, size_t size)
{
    Lexer lexer(string, size);
    // Skip the utf8 bom header if present.
    lexer.skip_word("\xEF\xBB\xBF");

    return node(lexer);
}

MarkupNode *markup_parse_file(const char *path)
{
    __cleanup(stream_cleanup) Stream *markup_file = stream_open(path, OPEN_READ);

    if (handle_has_error(markup_file))
    {
        return nullptr;
    }

    Lexer lexer(markup_file);
    // Skip the utf8 bom header if present.
    lexer.skip_word("\xEF\xBB\xBF");

    return node(lexer);
}
