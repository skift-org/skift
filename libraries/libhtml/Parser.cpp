#include <libhtml/Parser.h>

// https://www.w3.org/TR/2012/WD-html5-20120329/index.html#contents

namespace html
{

static constexpr const char *WHITESPACE = " \n\r\t";
static constexpr const char *COMMENT_START = "<!--";
static constexpr const char *COMMENT_END = "-->";
static constexpr const char *DOCTYPE_START = "<!DOCTYPE";
static constexpr const char *NEWLINE = "\r\n";
static constexpr const char *CLOSING_TAG = "</";
static constexpr const char *SELF_CLOSING_TAG = "/>";

void whitespace(Lexer &lexer)
{
    lexer.eat(WHITESPACE);
}

void comment(Lexer &lexer)
{
    lexer.skip_word(COMMENT_START);

    while (!lexer.current_is_word(COMMENT_END))
    {
        lexer.foreward();
    }

    lexer.skip_word(COMMENT_END);
}

void whitespace_or_comment(Lexer &lexer)
{
    whitespace(lexer);

    while (lexer.current_is_word(COMMENT_START))
    {
        comment(lexer);
        whitespace(lexer);
    }
}

void doctype(Lexer &lexer)
{
    lexer.skip_word(DOCTYPE_START);

    whitespace(lexer);

    assert(lexer.skip_word("html"));

    whitespace(lexer);

    lexer.skip('>');
}

void attribute(Lexer &lexer)
{
    __unused(lexer);
}

RefPtr<Node> start_tag(Lexer &lexer)
{
    lexer.skip('<');

    whitespace(lexer);

    attribute(lexer);

    if (lexer.current_is_word(SELF_CLOSING_TAG))
    {
        lexer.skip_word(SELF_CLOSING_TAG);
    }
    else
    {
        lexer.skip('>');
    }
}

void end_tag(Lexer &lexer, RefPtr<Node> node)
{
    lexer.skip_word(CLOSING_TAG);
    __unused(lexer);

    lexer.skip('>');
}

RefPtr<Node> element(Lexer &lexer)
{
    auto node = start_tag(lexer);

    if (!node->self_closing())
    {
        while (!lexer.current_is_word(CLOSING_TAG))
        {
            node->append(element(lexer));
        }

        end_tag(lexer, node);
    }

    return node;
}

RefPtr<Node> parse(Lexer &lexer)
{
    whitespace_or_comment(lexer);

    if (lexer.current_is_word(DOCTYPE_START))
    {
        doctype(lexer);
    }

    whitespace_or_comment(lexer);

    element(lexer);

    whitespace_or_comment(lexer);

    return nullptr;
}

} // namespace html
