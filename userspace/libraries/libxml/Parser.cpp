#include <libio/NumberScanner.h>
#include <libsystem/Logger.h>
#include <libxml/Parser.h>

ResultOr<String> read_comment(IO::Scanner &scan)
{
    StringBuilder builder{};
    scan.forward();
    while (scan.current() != '>')
    {
        builder.append(scan.current());
        if (scan.ended())
        {
            logger_error("Failed to \"read_comment\"");
            return Result::ERR_INVALID_DATA;
        }

        scan.forward();
    }

    return builder.finalize();
}

ResultOr<String> read_tag(IO::Scanner &scan)
{
    StringBuilder builder{};

    bool scan_name = true;
    // Read start-tag
    while (scan.current() != '>' && scan.do_continue())
    {
        if (scan.current() == ' ')
            scan_name = false;

        if (scan_name)
            builder.append(scan.current());

        //TODO: handle attributes
        scan.forward();
    }
    scan.forward();

    return builder.finalize();
}

ResultOr<String> read_end_tag(IO::Scanner &scan)
{
    scan.forward(2);
    return read_tag(scan);
}

ResultOr<String> read_start_tag(IO::Scanner &scan)
{
    scan.forward();
    return read_tag(scan);
}

Result read_node(IO::Scanner &scan, Xml::Node &node)
{
    scan.eat(Strings::WHITESPACE);

    if (scan.current() != '<')
    {
        logger_error("Failed to \"read_node\"");
        logger_error("Unexpected symbol: %i", scan.current());
        return Result::ERR_INVALID_DATA;
    }

    while (scan.do_continue())
    {
        char c = scan.peek(1);
        // Comment
        if (c == '!')
        {
            TRY(read_comment(scan));
        }
        // End-Tag
        else if (c == '/')
        {
            if (node.name().empty())
            {
                logger_error("End-tag without a start-tag");
                return Result::ERR_INVALID_DATA;
            }

            String end_tag = TRY(read_end_tag(scan));
            if (end_tag != node.name())
            {
                logger_error("End-tag does not match start-tag: ET %s ST %s", end_tag.cstring(), node.name().cstring());
                return Result::ERR_INVALID_DATA;
            }
            return Result::SUCCESS;
        }
        // Start-Tag
        else if (node.name().empty())
        {
            node.name() = TRY(read_start_tag(scan));

            StringBuilder builder{};
            while (scan.current() != '<')
            {
                builder.append(scan.current());
                scan.forward();
            }

            node.content() = builder.finalize();
        }
        // Child-Node
        else
        {
            TRY(read_node(scan, node.children().emplace_back()));
            scan.eat(Strings::WHITESPACE);
        }
    }

    return Result::SUCCESS;
}

Result read_declaration(IO::Scanner &scan, Xml::Declaration &)
{
    scan.eat(Strings::WHITESPACE);

    if (scan.current() != '<')
    {
        logger_error("Failed to \"read_declaration\"");
        logger_error("Unexpected symbol: %i", scan.current());
        return Result::ERR_INVALID_DATA;
    }

    if (scan.peek(1) != '?')
    {
        // We have no declaration
        return Result::SUCCESS;
    }

    while (scan.current() != '>' && scan.do_continue())
    {
        //TODO: read declaration content
        scan.forward();
    }

    return Result::SUCCESS;
}

FLATTEN ResultOr<Xml::Document> Xml::parse(IO::Reader &reader)
{
    IO::Scanner scan{reader};
    scan.skip_utf8bom();

    Xml::Document doc;
    TRY(read_declaration(scan, doc.declaration()));
    TRY(read_node(scan, doc.root()));

    return doc;
}