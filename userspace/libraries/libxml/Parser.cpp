#include <libio/Streams.h>
#include <libutils/Strings.h>
#include <libxml/Parser.h>

/* --- Attributes ----------------------------------------------------------- */

static String parse_attribute_name(IO::Scanner &scan)
{
    IO::MemoryWriter memory;

    while (scan.peek_is_any(Strings::ALL_ALPHA) ||
           scan.peek() == ':' ||
           scan.peek() == '-')
    {
        IO::write(memory, scan.next());
    }

    return memory.string();
}

static String parse_attribute_value(IO::Scanner &scan)
{
    IO::MemoryWriter memory;

    char ending = '\0';

    if (scan.skip('\''))
    {
        ending = '\'';
    }
    else if (scan.skip('\"'))
    {
        ending = '\"';
    }
    else
    {
        return "";
    }

    while (scan.peek() != ending &&
           !scan.ended())
    {
        IO::write(memory, scan.next());
    }

    scan.skip(ending);

    return memory.string();
}

// See https://www.w3.org/TR/xml/#NT-Attribute
static HjResult parse_attribute(IO::Scanner &scan, String &name, String &value)
{
    name = parse_attribute_name(scan);

    if (!scan.skip('='))
    {
        return ERR_INVALID_DATA;
    }

    value = parse_attribute_value(scan);

    return HjResult::SUCCESS;
}

/* --- Tag ------------------------------------------------------------------ */

static bool is_tag_end(IO::Scanner &scan)
{
    return scan.peek() == '>' || scan.peek_is_word("/>");
}

static String parse_tag_name(IO::Scanner &scan)
{
    IO::MemoryWriter memory;

    while (!is_tag_end(scan) &&
           !scan.peek_is_any(Strings::WHITESPACE) &&
           !scan.ended())
    {
        IO::write(memory, scan.next());
    }

    return memory.string();
}

// See https://www.w3.org/TR/xml/#dt-stag
static void parse_start_tag(IO::Scanner &scan, Xml::Node &node, bool &empty_tag)
{
    scan.skip('<');

    node.name(parse_tag_name(scan));

    scan.eat_any(Strings::WHITESPACE);

    while (!is_tag_end(scan) &&
           !scan.ended())
    {
        String name, value;
        parse_attribute(scan, name, value);
        node.attributes()[name] = value;

        scan.eat_any(Strings::WHITESPACE);
    }

    empty_tag = scan.skip_word("/>");

    if (!empty_tag)
    {
        scan.skip('>');
    }
}

// See https://www.w3.org/TR/xml/#dt-etag
static String parse_end_tag(IO::Scanner &scan)
{
    scan.skip_word("</");
    String name = parse_tag_name(scan);
    scan.eat_any(Strings::WHITESPACE);
    scan.skip('>');
    return name;
}

/* --- Node ----------------------------------------------------------------- */

static Xml::Node parse_node(IO::Scanner &scan);

// See https://www.w3.org/TR/xml/#NT-Comment
static Xml::Node parse_comment_node(IO::Scanner &scan)
{
    IO::MemoryWriter memory;
    scan.skip_word("<!--");

    while (!scan.peek_is_word("-->") &&
           !scan.ended())
    {
        IO::write(memory, scan.peek());
        scan.next();
    }

    scan.skip_word("-->");

    Xml::Node node;
    node.content(memory.string());
    return node;
}

static Xml::Node parse_tag_node(IO::Scanner &scan)
{
    Xml::Node node;
    bool self_closing;

    parse_start_tag(scan, node, self_closing);

    if (self_closing)
    {
        return node;
    }

    scan.eat_any(Strings::WHITESPACE);

    while (!scan.peek_is_word("</") &&
           !scan.ended())
    {
        node.children().push_back(parse_node(scan));
        scan.eat_any(Strings::WHITESPACE);
    }

    parse_end_tag(scan);

    return node;
}

static Xml::Node parse_text_node(IO::Scanner &scan)
{
    IO::logln("Starting text node {c}", scan.peek());

    Xml::Node node;
    IO::MemoryWriter memory{};

    while (scan.peek() != '<' &&
           !scan.ended())
    {
        IO::write(memory, scan.next());
    }

    node.content(memory.string());
    IO::logln("Ending text node {c}", scan.peek());
    return node;
}

// See https://www.w3.org/TR/xml/#sec-logical-struct
static Xml::Node parse_node(IO::Scanner &scan)
{
    if (scan.peek_is_word("<!--"))
    {
        return parse_comment_node(scan);
    }
    else if (scan.peek() == '<')
    {
        return parse_tag_node(scan);
    }
    else
    {
        return parse_text_node(scan);
    }
}

/* --- Declaration ---------------------------------------------------------- */

// See https://www.w3.org/TR/xml/#sec-prolog-dtd
static Xml::Declaration parse_declaration(IO::Scanner &scan)
{
    Xml::Declaration declaration;

    scan.skip_word("<?");

    while (!scan.peek_is_word("?>") &&
           !scan.ended())
    {
        //TODO: read declaration content
        scan.next();
    }

    scan.skip_word("?>");

    return declaration;
}

ResultOr<Xml::Document> Xml::parse(IO::Reader &reader)
{
    IO::Scanner scan{reader};
    Xml::Document document;

    scan.skip_utf8bom();

    scan.eat_any(Strings::WHITESPACE);

    if (scan.peek_is_word("<?"))
    {
        document.declaration(parse_declaration(scan));
    }
    scan.eat_any(Strings::WHITESPACE);

    if (scan.peek() == '<')
    {
        document.root(parse_node(scan));
    }

    return document;
}
