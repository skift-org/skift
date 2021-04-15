#include <libio/NumberScanner.h>
#include <libsystem/Logger.h>
#include <libutils/HashMap.h>
#include <libxml/Parser.h>

//See https://www.w3.org/TR/xml/#NT-Comment
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

// See https://www.w3.org/TR/xml/#NT-Attribute
Result read_attribute(IO::Scanner &scan, String &name, String &value)
{
    StringBuilder builder{};

    // Attribute name
    while (scan.current_is(Strings::ALL_ALPHA) || (scan.current() == ':'))
    {
        builder.append(scan.current());
        scan.forward();
    }
    name = builder.finalize();

    // Attribute equal
    if (!scan.skip('='))
    {
        logger_error("Expected = after attribute name: %i", scan.current());
        return Result::ERR_INVALID_DATA;
    }

    // TODO: handle single quotes
    if (!scan.skip('\"'))
    {
        logger_error("Expected \" to start an attribute value");
        return Result::ERR_INVALID_DATA;
    }

    // Attribute value
    while (scan.current() != '\"')
    {
        builder.append(scan.current());
        scan.forward();
    }
    value = builder.finalize();

    return Result::SUCCESS;
}

template <bool has_attributes>
ResultOr<String> read_tag(IO::Scanner &scan, HashMap<String, String> &attributes, bool &empty_tag)
{
    StringBuilder builder{};

    // Read tag-name
    bool scan_name = true;
    while (scan.current() != '>' && !scan.current_is_word("/>") && scan.do_continue())
    {
        // There may be no whitespace before the tagname
        // See https://www.w3.org/TR/REC-xml/#sec-starttags
        if (scan_name)
        {
            if (scan.current() == ' ')
            {
                logger_error("Whitespaces before tagname not allowed");
                return Result::ERR_INVALID_DATA;
            }

            builder.append(scan.current());
        }
        else if (scan.current() != ' ')
        {
            if constexpr (has_attributes)
            {
                String name, value;
                TRY(read_attribute(scan, name, value));
                attributes[name] = value;
            }
            else
            {
                logger_error("Unexpected symbol in end-tag: %i", scan.current());
                return Result::ERR_INVALID_DATA;
            }
        }

        scan.forward();

        if (scan.current() == ' ')
            scan_name = false;
    }

    // If the tag ended with /> it was an empty tag
    empty_tag = scan.skip_word("/>");

    scan.forward();

    return builder.finalize();
}

// See https://www.w3.org/TR/xml/#dt-etag
ResultOr<String> read_end_tag(IO::Scanner &scan)
{
    scan.forward(2);
    HashMap<String, String> tmp_attributes;
    bool tmp_empty_tag;
    return read_tag<false>(scan, tmp_attributes, tmp_empty_tag);
}

// See https://www.w3.org/TR/xml/#dt-stag
// TODO: this can also be an empty_tag
ResultOr<String> read_start_tag(IO::Scanner &scan, HashMap<String, String> &attributes, bool &empty_tag)
{
    scan.forward();
    return read_tag<true>(scan, attributes, empty_tag);
}

// See https://www.w3.org/TR/xml/#sec-logical-struct
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
        // Start-Tag & Empty-Tag
        else if (node.name().empty())
        {
            bool empty_tag;
            node.name() = TRY(read_start_tag(scan, node.attributes(), empty_tag));

            // It was an empty tag, so we have no content / end-tag
            if (empty_tag)
            {
                return Result::SUCCESS;
            }

            // Read the content for the tag
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

// See https://www.w3.org/TR/xml/#sec-prolog-dtd
Result read_declaration(IO::Scanner &scan, Xml::Declaration &)
{
    scan.eat(Strings::WHITESPACE);

    if (scan.current() != '<')
    {
        logger_error("Failed to \"read_declaration\"");
        logger_error("Unexpected symbol: %i", scan.current());
        return Result::ERR_INVALID_DATA;
    }

    // We have no declaration
    if (scan.peek(1) != '?')
    {
        logger_warn("Missing XML declaration");
        return Result::SUCCESS;
    }

    // declaration ends with ?>
    while (!scan.current_is_word("?>") && scan.do_continue())
    {
        //TODO: read declaration content
        scan.forward();
    }

    // Skip ?>
    if (!scan.skip_word("?>"))
    {
        logger_error("Failed to \"read_declaration\"");
        return Result::ERR_INVALID_DATA;
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