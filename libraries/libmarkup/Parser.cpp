#include <assert.h>
#include <libsystem/Logger.h>
#include <libutils/unicode/Codepoint.h>
#include <libutils/NumberParser.h>
#include <libutils/Scanner.h>
#include <libutils/ScannerUtils.h>
#include <libutils/StringBuilder.h>
#include <libutils/Strings.h>
#include <string.h>

#include <libmarkup/Markup.h>

namespace markup
{

static void whitespace(Scanner &scan)
{
    scan.eat(Strings::WHITESPACE);
}

static String identifier(Scanner &scan)
{
    StringBuilder builder{};

    while (scan.current_is(Strings::ALL_ALPHA) &&
           scan.do_continue())
    {
        builder.append(scan.current());
        scan.foreward();
    }

    return builder.finalize();
}

static String string(Scanner &scan)
{
    StringBuilder builder{};

    scan.skip('"');

    while (scan.do_continue() &&
           scan.current() != '"')
    {
        if (scan.current() == '\\')
        {
            builder.append(scan_json_escape_sequence(scan));
        }
        else
        {
            builder.append(scan.current());
            scan.foreward();
        }
    }

    scan.skip('"');

    return builder.finalize();
}

static void attribute(Scanner &scan, Attributes &attr)
{
    auto ident = identifier(scan);

    whitespace(scan);

    if (scan.skip('='))
    {
        whitespace(scan);
        attr[ident] = string(scan);
    }
    else
    {
        attr[ident] = "";
    }
}

static Node opening_tag(Scanner &scan)
{
    if (!scan.skip('<'))
    {
        return {"error"};
    }

    whitespace(scan);

    auto type = identifier(scan);

    whitespace(scan);

    Attributes attr{};

    while (scan.current_is(Strings::ALL_ALPHA) &&
           scan.do_continue())
    {
        attribute(scan, attr);
        whitespace(scan);
    }

    auto flags = Node::NONE;

    if (scan.skip('/'))
    {
        scan.skip('>');

        flags = Node::SELF_CLOSING;
    }

    scan.skip('>');

    return {type, flags, move(attr)};
}

static void closing_tag(Scanner &scan, const Node &node)
{
    scan.skip('<');
    scan.skip('/');

    whitespace(scan);

    auto type = identifier(scan);

    if (!node.is(type))
    {
        logger_warn(
            "Opening tag <%s> doesn't match closing tag </%s>",
            node.type().cstring(),
            type.cstring());
    }

    whitespace(scan);

    scan.skip('>');
}

static Node node(Scanner &scan)
{
    whitespace(scan);

    auto n = opening_tag(scan);

    if (n.flags() & Node::SELF_CLOSING)
    {
        return n;
    }

    whitespace(scan);

    while (scan.peek(0) == '<' &&
           scan.peek(1) != '/' &&
           scan.do_continue())
    {
        n.add_child(node(scan));
        whitespace(scan);
    }

    whitespace(scan);

    closing_tag(scan, n);

    return n;
}

Node parse(Scanner &scan)
{
    scan_skip_utf8bom(scan);
    return node(scan);
}

Node parse_file(String path)
{
    __cleanup(stream_cleanup) Stream *json_file = stream_open(path.cstring(), OPEN_READ | OPEN_BUFFERED);

    if (handle_has_error(json_file))
    {
        return {"error"};
    }

    StreamScanner scan{json_file};
    scan_skip_utf8bom(scan);
    return node(scan);
}

} // namespace markup
