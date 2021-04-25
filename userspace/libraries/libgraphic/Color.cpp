#include <string.h>

#include <libgraphic/Color.h>
#include <libutils/NumberParser.h>
#include <libutils/Scanner.h>
#include <libutils/ScannerUtils.h>

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

static void whitespace(Scanner &scan)
{
    scan.eat(Strings::WHITESPACE);
}

static double number(Scanner &scan)
{
    return scan_float(scan);
}

Color Color::parse(String string)
{
    StringScanner scan{string.cstring(), string.length()};

    auto parse_Element = [&](StringScanner &scan) {
        whitespace(scan);

        auto value = number(scan);

        whitespace(scan);

        if (scan.skip('%'))
        {
            value /= 100;
        }
        else
        {
            value /= 255;
        }

        whitespace(scan);

        return value;
    };

    if (scan.skip_word("#"))
    {
        size_t length = 0;
        char buffer[9];

        while (scan.current_is(Strings::ALL_XDIGITS) && length < 8)
        {
            buffer[length] = scan.current();
            length++;

            scan.foreward();
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

        return from_hexa(parse_uint_inline(PARSER_HEXADECIMAL, hex, 0));
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

        whitespace(scan);
        auto alpha = number(scan);
        whitespace(scan);

        scan.skip(')');

        return Color::from_rgba(red, green, blue, alpha);
    }
    else if (scan.skip_word("hsl"))
    {
        scan.skip('(');

        whitespace(scan);
        auto hue = number(scan);
        whitespace(scan);

        scan.skip(',');

        whitespace(scan);
        auto saturation = number(scan);
        whitespace(scan);

        scan.skip(',');

        whitespace(scan);
        auto value = number(scan);
        whitespace(scan);

        scan.skip(')');

        return Color::from_hsv(hue, saturation, value);
    }
    else if (scan.skip_word("hsla"))
    {
        scan.skip('(');

        whitespace(scan);
        auto hue = number(scan);
        whitespace(scan);

        scan.skip(',');

        whitespace(scan);
        auto saturation = number(scan);
        whitespace(scan);

        scan.skip(',');

        whitespace(scan);
        auto value = number(scan);
        whitespace(scan);

        scan.skip(',');

        whitespace(scan);
        auto alpha = number(scan);
        whitespace(scan);

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
