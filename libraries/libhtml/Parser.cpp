#include <libhtml/Parser.h>

namespace html
{

static constexpr const char *WHITESPACE = " \n\r\t";
static constexpr const char *COMMENT_START = "<!--";
static constexpr const char *COMMENT_END = "-->";
static constexpr const char *DOCTYPE_START = "<!DOCTYPE";

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

void start_tag(Lexer &lexer)
{
    lexer.skip('<');

    whitespace(lexer);

    attribute(lexer);

    lexer.skip('>');
}

void end_tag(Lexer &lexer)
{
    __unused(lexer);
}

void element(Lexer &lexer)
{
    __unused(lexer);
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
