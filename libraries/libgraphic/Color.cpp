
#include <libgraphic/Color.h>
#include <libsystem/core/CString.h>
#include <libsystem/utils/NumberParser.h>
#include <libutils/Scanner.h>

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

static constexpr const char *DIGITS = "0123456789";
static constexpr const char *HEXDIGITS = "0123456789abcdef";
static constexpr const char *HEXDIGITS_MAj = "0123456789ABCDEF";

static void whitespace(Scanner &scan)
{
    scan.eat(" \n\r\t");
}

static double number(Scanner &scan)
{
    int ipart = 0;
    double fpart = 0;

    if (scan.current_is(DIGITS))
    {
        while (scan.current_is(DIGITS))
        {
            ipart *= 10;
            ipart += scan.current() - '0';
            scan.foreward();
        }
    }

    if (scan.skip('.'))
    {
        double multiplier = 0.1;

        while (scan.current_is(DIGITS))
        {
            fpart += multiplier * (scan.current() - '0');
            multiplier *= 0.1;
            scan.foreward();
        }
    }

    return ipart + fpart;
}

Color Color::parse(const char *name)
{
    StringScanner scan{name, strlen(name)};

    if (scan.skip_word("#"))
    {
        size_t length = 0;
        char buffer[9];

        while ((scan.current_is(HEXDIGITS) || scan.current_is(HEXDIGITS_MAj)) && length < 8)
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

        whitespace(scan);
        auto red = number(scan);
        whitespace(scan);

        scan.skip(',');

        whitespace(scan);
        auto green = number(scan);
        whitespace(scan);

        scan.skip(',');

        whitespace(scan);
        auto blue = number(scan);
        whitespace(scan);

        scan.skip(')');

        return Color::from_rgb(red, green, blue);
    }
    else if (scan.skip_word("rgba"))
    {
        scan.skip('(');

        whitespace(scan);
        auto red = number(scan);
        whitespace(scan);

        scan.skip(',');

        whitespace(scan);
        auto green = number(scan);
        whitespace(scan);

        scan.skip(',');

        whitespace(scan);
        auto blue = number(scan);
        whitespace(scan);

        scan.skip(',');

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
        for (size_t i = 0; i < __array_length(_color_names); i++)
        {
            if (strcmp(name, _color_names[i].name) == 0)
            {
                return _color_names[i].color;
            }
        }
    }

    return Colors::MAGENTA;
}
