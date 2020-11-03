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

void whitespace(Scanner&scan)
{
    scan.eat(WHITESPACE);
}

void comment(Scanner&scan)
{
    scan.skip_word(COMMENT_START);

    while (!scan.current_is_word(COMMENT_END))
    {
        scan.foreward();
    }

    scan.skip_word(COMMENT_END);
}

void whitespace_or_comment(Scanner&scan)
{
    whitespace(scan);

    while (scan.current_is_word(COMMENT_START))
    {
        comment(scan);
        whitespace(scan);
    }
}

void doctype(Scanner&scan)
{
    scan.skip_word(DOCTYPE_START);

    whitespace(scan);

    assert(scan.skip_word("html"));

    whitespace(scan);

    scan.skip('>');
}

void attribute(Scanner&scan)
{
    __unused(scan);
}

RefPtr<Node> start_tag(Scanner&scan)
{
    scan.skip('<');

    whitespace(scan);

    attribute(scan);

    if (scan.current_is_word(SELF_CLOSING_TAG))
    {
        scan.skip_word(SELF_CLOSING_TAG);
    }
    else
    {
        scan.skip('>');
    }

    return make<Node>();
}

void end_tag(Scanner&scan, RefPtr<Node> node)
{
    __unused(node);

    scan.skip_word(CLOSING_TAG);

    scan.skip('>');
}

RefPtr<Node> element(Scanner&scan)
{
    auto node = start_tag(scan);

    if (!node->self_closing())
    {
        while (!scan.current_is_word(CLOSING_TAG))
        {
            node->append(element(scan));
        }

        end_tag(scan, node);
    }

    return node;
}

RefPtr<Node> parse(Scanner&scan)
{
    whitespace_or_comment(scan);

    if (scan.current_is_word(DOCTYPE_START))
    {
        doctype(scan);
    }

    whitespace_or_comment(scan);

    element(scan);

    whitespace_or_comment(scan);

    return nullptr;
}

} // namespace html
