
#include <libgraphic/Color.h>
#include <libsystem/core/CString.h>
#include <libsystem/utils/NumberParser.h>

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

static void whitespace(Lexer &lexer)
{
    lexer.eat(" \n\r\t");
}

static constexpr const char *DIGITS = "0123456789";
static constexpr const char *HEXDIGITS = "0123456789abcdef";
static constexpr const char *HEXDIGITS_MAj = "0123456789ABCDEF";

static double number(Lexer &lexer)
{
    int ipart = 0;
    double fpart = 0;

    if (lexer.current_is(DIGITS))
    {
        while (lexer.current_is(DIGITS))
        {
            ipart *= 10;
            ipart += lexer.current() - '0';
            lexer.foreward();
        }
    }

    if (lexer.skip('.'))
    {
        double multiplier = 0.1;

        while (lexer.current_is(DIGITS))
        {
            fpart += multiplier * (lexer.current() - '0');
            multiplier *= 0.1;
            lexer.foreward();
        }
    }

    return ipart + fpart;
}

Color Color::parse(const char *name)
{
    Lexer lexer{name, strlen(name)};

    if (lexer.skip_word("#"))
    {
        size_t length = 0;
        char buffer[9];

        while ((lexer.current_is(HEXDIGITS) || lexer.current_is(HEXDIGITS_MAj)) && length < 8)
        {
            buffer[length] = lexer.current();
            length++;

            lexer.foreward();
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

        uint32_t value = parse_uint_inline(PARSER_HEXADECIMAL, hex, 0);

        logger_trace("PARSE: '%s' -> '%s' %d = %08x", buffer, hex, length, value);

        return from_hexa(value);
    }
    else if (lexer.skip_word("rgb"))
    {
        lexer.skip('(');

        whitespace(lexer);
        auto red = number(lexer);
        whitespace(lexer);

        lexer.skip(',');

        whitespace(lexer);
        auto green = number(lexer);
        whitespace(lexer);

        lexer.skip(',');

        whitespace(lexer);
        auto blue = number(lexer);
        whitespace(lexer);

        lexer.skip(')');

        return Color::from_rgb(red, green, blue);
    }
    else if (lexer.skip_word("rgba"))
    {
        lexer.skip('(');

        whitespace(lexer);
        auto red = number(lexer);
        whitespace(lexer);

        lexer.skip(',');

        whitespace(lexer);
        auto green = number(lexer);
        whitespace(lexer);

        lexer.skip(',');

        whitespace(lexer);
        auto blue = number(lexer);
        whitespace(lexer);

        lexer.skip(',');

        whitespace(lexer);
        auto alpha = number(lexer);
        whitespace(lexer);

        lexer.skip(')');

        return Color::from_rgba(red, green, blue, alpha);
    }
    else if (lexer.skip_word("hsl"))
    {
        lexer.skip('(');

        whitespace(lexer);
        auto hue = number(lexer);
        whitespace(lexer);

        lexer.skip(',');

        whitespace(lexer);
        auto saturation = number(lexer);
        whitespace(lexer);

        lexer.skip(',');

        whitespace(lexer);
        auto value = number(lexer);
        whitespace(lexer);

        lexer.skip(')');

        return Color::from_hsv(hue, saturation, value);
    }
    else if (lexer.skip_word("hsla"))
    {
        lexer.skip('(');

        whitespace(lexer);
        auto hue = number(lexer);
        whitespace(lexer);

        lexer.skip(',');

        whitespace(lexer);
        auto saturation = number(lexer);
        whitespace(lexer);

        lexer.skip(',');

        whitespace(lexer);
        auto value = number(lexer);
        whitespace(lexer);

        lexer.skip(',');

        whitespace(lexer);
        auto alpha = number(lexer);
        whitespace(lexer);

        lexer.skip(')');

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
