#pragma once

#include <ctype.h>
#include <math.h>
#include <string.h>

#include <libio/NumberScanner.h>
#include <libio/Scanner.h>
#include <libio/Write.h>
#include <libio/Writer.h>
#include <libutils/Strings.h>

namespace IO
{

enum struct Sign
{
    ALWAYS,
    ONLY_NEGATIVE,
    SPACE_FOR_POSITIVE,
};

enum struct Type
{
    DEFAULT,

    STRING,
    CHARACTER,

    BINARY,
    DECIMAL,
    OCTAL,
    HEXADECIMAL,
};

struct Formatter
{
private:
    Type _type = Type::DEFAULT;
    Optional<int> _width;
    char _padding = ' ';
    int _precision = 4;
    bool _capitalized = false;
    Sign _sign = Sign::ONLY_NEGATIVE;

    int base()
    {
        switch (_type)
        {
        case Type::BINARY:
            return 2;

        case Type::OCTAL:
            return 8;

        case Type::HEXADECIMAL:
            return 16;

        default:
            return 10;
        }
    }

    char digit(int digit)
    {
        assert(digit >= 0 && digit < base());

        if (_capitalized)
        {
            return Strings::LOWERCASE_XDIGITS[digit % base()];
        }
        else
        {
            return Strings::UPPERCASE_XDIGITS[digit % base()];
        }
    }

    ResultOr<size_t> sign(Writer &writer, auto value)
    {
        if (value < 0)
        {
            return write(writer, '-');
        }
        else if (_sign == Sign::ALWAYS)
        {
            return write(writer, '+');
        }
        else if (_sign == Sign::SPACE_FOR_POSITIVE)
        {
            return write(writer, ' ');
        }

        return 0;
    }

    ResultOr<size_t> format_rune(Writer &writer, Text::Rune rune)
    {
        uint8_t buffer[5];
        int length = Text::rune_to_utf8(rune, buffer);
        return format_string(writer, (const char *)buffer, length);
    }

    ResultOr<size_t> format_string(Writer &writer, const char *string, size_t length)
    {
        return writer.write(string, length);
    }

    ResultOr<size_t> format_signed(Writer &writer, int64_t value)
    {
        size_t written = 0;

        written += TRY(sign(writer, value));

        if (value < 0)
        {
            value = -value;
        }

        return written + TRY(format_unsigned(writer, value));
    }

    ResultOr<size_t> format_unsigned(Writer &writer, uint64_t value)
    {
        if (value == 0)
        {
            return write(writer, '0');
        }

        size_t i = 0;
        char buffer[64] = {};

        while (value != 0)
        {
            buffer[i] = digit(value % base());
            value /= base();
            i++;
        }

        strrvs(buffer);

        return writer.write(buffer, i);
    }

#ifndef __KERNEL__

    ResultOr<size_t> format_float(Writer &writer, double value)
    {
        if (isnan(value))
        {
            return write(writer, _capitalized ? "NaN" : "nan");
        }

        size_t written = 0;

        written += TRY(sign(writer, value));

        if (value < 0)
        {
            value = -value;
        }

        if (isinf(value))
        {
            return write(writer, _capitalized ? "INF" : "inf");
        }

        int64_t ipart = (int64_t)value;
        written += TRY(format_unsigned(writer, ipart));

        if (_precision > 0)
        {
            written += TRY(write(writer, '.'));

            double fpart = value - (double)ipart;

            for (int i = 0; i < _precision; i++)
            {
                fpart *= base();
                written += TRY(write(writer, digit((int)fpart % base())));
            }
        }

        return written;
    }

#endif

public:
    static Formatter octal() { return {Type::OCTAL, 4, false}; }
    static Formatter decimal() { return {Type::DECIMAL, 4, false}; }
    static Formatter hexadecimal() { return {Type::HEXADECIMAL, 4, false}; }

    static Formatter parse(Scanner &scan)
    {
        Formatter fmt;

        scan.skip('{');

        if (scan.skip('0'))
        {
            fmt = fmt.padding('0');
        }

        if (scan.peek_is_any(Strings::DIGITS))
        {
            fmt = fmt.width(NumberScanner::decimal().scan_uint(scan).unwrap());
        }

        if (scan.peek_is_any("scbdoxp"))
        {
            char c = scan.next();

            if (isupper(c))
            {
                fmt = fmt.capitalized();
                c = tolower(c);
            }

            switch (c)
            {
            case 's':
                fmt = fmt.type(Type::STRING);
                break;

            case 'c':
                fmt = fmt.type(Type::CHARACTER);
                break;

            case 'b':
                fmt = fmt.type(Type::BINARY);
                break;

            case 'd':
                fmt = fmt.type(Type::DECIMAL);
                break;

            case 'o':
                fmt = fmt.type(Type::OCTAL);
                break;

            case 'x':
                fmt = fmt.type(Type::HEXADECIMAL);
                break;

            case 'p':
                fmt = fmt.type(Type::HEXADECIMAL);
                break;

            default:
                break;
            }
        }

        while (!scan.ended() && scan.peek() != '}')
        {
            scan.next();
        }

        scan.skip('}');

        return fmt;
    }

    [[nodiscard]] Formatter type(Type value)
    {
        Formatter copy = *this;
        copy._type = value;
        return copy;
    }

    [[nodiscard]] Formatter padding(char c)
    {
        Formatter copy = *this;
        copy._padding = c;
        return copy;
    }

    [[nodiscard]] Formatter width(int value)
    {
        Formatter copy = *this;
        copy._width = value;
        return copy;
    }

    [[nodiscard]] Formatter precision(int value)
    {
        Formatter copy = *this;
        copy._precision = value;
        return copy;
    }

    [[nodiscard]] Formatter capitalized()
    {
        Formatter copy = *this;
        copy._capitalized = true;
        return copy;
    }

    [[nodiscard]] Formatter sign(Sign sign)
    {
        Formatter copy = *this;
        copy._sign = sign;
        return copy;
    }

    Formatter()
    {
    }

    Formatter(Type type, int precision, bool capitalized)
        : _type{type}, _precision{precision}, _capitalized{capitalized}
    {
    }

    ResultOr<size_t> format(Writer &writer, char value)
    {
        return format(writer, (long)value);
    }

    ResultOr<size_t> format(Writer &writer, unsigned char value)
    {
        return format(writer, (unsigned long)value);
    }

    ResultOr<size_t> format(Writer &writer, short value)
    {
        return format(writer, (long)value);
    }

    ResultOr<size_t> format(Writer &writer, unsigned short value)
    {
        return format(writer, (unsigned long)value);
    }

    ResultOr<size_t> format(Writer &writer, int value)
    {
        return format(writer, (long)value);
    }

    ResultOr<size_t> format(Writer &writer, unsigned int value)
    {
        return format(writer, (unsigned long)value);
    }

    ResultOr<size_t> format(Writer &writer, long int value)
    {
        return format(writer, (long long)value);
    }

    ResultOr<size_t> format(Writer &writer, unsigned long int value)
    {
        return format(writer, (unsigned long long)value);
    }

    ResultOr<size_t> format(Writer &writer, long long int value)
    {
        if (_type == Type::CHARACTER)
        {
            if (value < 0)
            {
                return format_rune(writer, U'�');
            }
            else
            {
                return format_rune(writer, value);
            }
        }
        else
        {
            return format_signed(writer, value);
        }
    }

    ResultOr<size_t> format(Writer &writer, unsigned long long int value)
    {
        if (_type == Type::CHARACTER)
        {
            return format_rune(writer, value);
        }
        else
        {
            return format_unsigned(writer, value);
        }
    }

#ifndef __KERNEL__

    ResultOr<size_t> format(Writer &writer, float value)
    {
        return format_float(writer, value);
    }

    ResultOr<size_t> format(Writer &writer, double value)
    {
        return format_float(writer, value);
    }

#endif

    template <typename T>
    ResultOr<size_t> format(Writer &writer, const T *ptr)
    {
        return format(writer, reinterpret_cast<uintptr_t>(ptr));
    }

    ResultOr<size_t> format(Writer &writer, const char *str)
    {
        return format_string(writer, str, strlen(str));
    }

    ResultOr<size_t> format(Writer &writer, std::nullptr_t)
    {
        return format_string(writer, "nullptr", 7);
    }

    ResultOr<size_t> format(Writer &writer, const String str)
    {
        return format_string(writer, str.cstring(), str.length());
    }

    ResultOr<size_t> format(Writer &writer, const Slice slice)
    {
        return format_string(writer, (const char *)slice.start(), slice.size());
    }

    ResultOr<size_t> format(Writer &writer, ToString auto &to_string)
    {
        return format(writer, to_string.string());
    }
};

} // namespace IO
