#include <libio/NumberScanner.h>
#include <libsystem/Logger.h>
#include <libxml/Parser.h>

ResultOr<String> read_comment(IO::Scanner &scan)
{
    logger_info("Begin: \"read_comment\"");
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
    logger_info("Begin: \"read_end_tag\"");
    scan.forward(2);
    return read_tag(scan);
}

ResultOr<String> read_start_tag(IO::Scanner &scan)
{
    logger_info("Begin: \"read_start_tag\"");
    scan.forward();
    return read_tag(scan);
}

Result read_node(IO::Scanner &scan, Xml::Node &node)
{
    logger_info("Begin: \"read_node\"");
    scan.eat(Strings::WHITESPACE);

    if (scan.current() != '<')
    {
        logger_error("Failed to \"read_node\"");
        logger_error("Unexpected symbol: %i", scan.current());
        return Result::ERR_INVALID_DATA;
    }

    bool is_done = false;

    while (!is_done && scan.do_continue())
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
            is_done = true;
        }
        // Start-Tag
        else if (node.name().empty())
        {
            node.name() = TRY(read_start_tag(scan));
            logger_info("node.name: %s", node.name().cstring());

            StringBuilder builder{};
            while (scan.current() != '<')
            {
                builder.append(scan.current());
                scan.forward();
            }

            node.content() = builder.finalize();
            logger_info("node.content: %s", node.content().cstring());
        }
        // Child-Node
        else
        {
            TRY(read_node(scan, node.children().emplace_back()));
        }
    }

    return Result::SUCCESS;
}

FLATTEN ResultOr<Xml::Document> Xml::parse(IO::Reader &reader)
{
    IO::Scanner scan{reader};
    scan.skip_utf8bom();

    Xml::Document doc;
    TRY(read_node(scan, doc.root()));

    return doc;
}