#include <string.h>

#include <libgraphic/Color.h>
#include <libio/MemoryReader.h>
#include <libio/NumberScanner.h>
#include <libio/Scanner.h>
#include <libutils/Strings.h>

namespace Graphic
{

struct ColorName
{
    const char *name;
    size_t size;
    const Color color;
};

static constexpr ColorName _color_names[] = {
#define __ENTRY(__name, __value) {#__name, sizeof(#__name), Color::from_hex(__value)},
    COLOR_LIST(__ENTRY)
#undef __ENTRY
};

static void parse_whitespace(IO::Scanner &scan)
{
    scan.eat_any(Strings::WHITESPACE);
}

static double parse_number(IO::Scanner &scan)
{
    return IO::NumberScanner::decimal().scan_float(scan).unwrap_or(0);
}

Color Color::parse(String string)
{
    IO::MemoryReader memory{string};
    IO::Scanner scan{memory};

    auto parse_Element = [&](IO::Scanner &scan) {
        parse_whitespace(scan);

        auto value = parse_number(scan);

        parse_whitespace(scan);

        if (scan.skip('%'))
        {
            value /= 100;
        }
        else
        {
            value /= 255;
        }

        parse_whitespace(scan);

        return value;
    };

    if (scan.skip_word("#"))
    {
        size_t length = 0;
        char buffer[9];

        while (scan.peek_is_any(Strings::ALL_XDIGITS) && length < 8)
        {
            buffer[length] = scan.next();
            length++;
        }

        buffer[length] = '\0';

        char hex[9] = {};

        if (length == 1)
        {
            hex[0] = buffer[0];
            hex[1] = buffer[0];
            hex[2] = buffer[0];
            hex[3] = buffer[0];
            hex[4] = buffer[0];
            hex[5] = buffer[0];
            hex[6] = 'f';
            hex[7] = 'f';
        }
        else if (length == 3)
        {

            hex[0] = buffer[0];
            hex[1] = buffer[0];
            hex[2] = buffer[1];
            hex[3] = buffer[1];
            hex[4] = buffer[2];
            hex[5] = buffer[2];
            hex[6] = 'f';
            hex[7] = 'f';
        }
        else if (length == 4)
        {
            hex[0] = buffer[0];
            hex[1] = buffer[0];
            hex[2] = buffer[1];
            hex[3] = buffer[1];
            hex[4] = buffer[2];
            hex[5] = buffer[2];
            hex[6] = buffer[3];
            hex[7] = buffer[3];
        }
        else if (length == 6)
        {
            hex[0] = buffer[0];
            hex[1] = buffer[1];
            hex[2] = buffer[2];
            hex[3] = buffer[3];
            hex[4] = buffer[4];
            hex[5] = buffer[5];
            hex[6] = 'f';
            hex[7] = 'f';
        }
        else if (length == 8)
        {
            hex[0] = buffer[0];
            hex[1] = buffer[1];
            hex[2] = buffer[2];
            hex[3] = buffer[3];
            hex[4] = buffer[4];
            hex[5] = buffer[5];
            hex[6] = buffer[6];
            hex[7] = buffer[7];
        }

        IO::MemoryReader memory{hex};
        IO::Scanner hexscan{memory};
        return from_hexa(IO::NumberScanner::hexadecimal().scan_uint(hexscan).unwrap_or(0));
    }
    else if (scan.skip_word("rgb"))
    {
        scan.skip('(');

        auto red = parse_Element(scan);
        auto green = parse_Element(scan);
        auto blue = parse_Element(scan);

        scan.skip(')');

        return Color::from_rgb(red, green, blue);
    }
    else if (scan.skip_word("rgba"))
    {
        scan.skip('(');

        auto red = parse_Element(scan);
        auto green = parse_Element(scan);
        auto blue = parse_Element(scan);

        parse_whitespace(scan);
        auto alpha = parse_number(scan);
        parse_whitespace(scan);

        scan.skip(')');

        return Color::from_rgba(red, green, blue, alpha);
    }
    else if (scan.skip_word("hsl"))
    {
        scan.skip('(');

        parse_whitespace(scan);
        auto hue = parse_number(scan);
        parse_whitespace(scan);

        scan.skip(',');

        parse_whitespace(scan);
        auto saturation = parse_number(scan);
        parse_whitespace(scan);

        scan.skip(',');

        parse_whitespace(scan);
        auto value = parse_number(scan);
        parse_whitespace(scan);

        scan.skip(')');

        return Color::from_hsv(hue, saturation, value);
    }
    else if (scan.skip_word("hsla"))
    {
        scan.skip('(');

        parse_whitespace(scan);
        auto hue = parse_number(scan);
        parse_whitespace(scan);

        scan.skip(',');

        parse_whitespace(scan);
        auto saturation = parse_number(scan);
        parse_whitespace(scan);

        scan.skip(',');

        parse_whitespace(scan);
        auto value = parse_number(scan);
        parse_whitespace(scan);

        scan.skip(',');

        parse_whitespace(scan);
        auto alpha = parse_number(scan);
        parse_whitespace(scan);

        scan.skip(')');

        return Color::from_hsva(hue, saturation, value, alpha);
    }
    else
    {
        for (const auto &_color_name : _color_names)
        {
            if (string == _color_name.name)
            {
                return _color_name.color;
            }
        }
    }

    return Colors::MAGENTA;
}

} // namespace Graphic
