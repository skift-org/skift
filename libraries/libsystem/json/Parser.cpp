#include <libsystem/Assert.h>
#include <libsystem/core/CString.h>
#include <libsystem/io/Stream.h>
#include <libsystem/json/Json.h>
#include <libsystem/math/Math.h>
#include <libsystem/unicode/Codepoint.h>
#include <libsystem/utils/NumberParser.h>
#include <libutils/Scanner.h>
#include <libutils/ScannerUtils.h>
#include <libutils/StringBuilder.h>

namespace json
{
static constexpr const char *WHITESPACE = " \n\r\t";
static constexpr const char *DIGITS = "0123456789";
static constexpr const char *ALPHA = "abcdefghijklmnopqrstuvwxyz";

static Value value(Scanner &scan);

static void whitespace(Scanner &scan)
{
    scan.eat(WHITESPACE);
}

static int digits(Scanner &scan)
{
    int digits = 0;

    while (scan.current_is(DIGITS))
    {
        digits *= 10;
        digits += scan.current() - '0';
        scan.foreward();
    }

    return digits;
}

static Value number(Scanner &scan)
{
    int ipart_sign = 1;

    if (scan.skip('-'))
    {
        ipart_sign = -1;
    }

    int ipart = 0;

    if (scan.current_is(DIGITS))
    {
        ipart = digits(scan);
    }

#ifdef __KERNEL__
    return {ipart_sign * ipart};
#else

    double fpart = 0;

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

    int exp = 0;

    if (scan.current_is("eE"))
    {
        scan.foreward();
        int exp_sign = 1;

        if (scan.current() == '-')
        {
            exp_sign = -1;
        }

        if (scan.current_is("+-"))
            scan.foreward();

        exp = digits(scan) * exp_sign;
    }

    if (fpart == 0 && exp >= 0)
    {
        return {ipart_sign * ipart * (int)pow(10, exp)};
    }
    else
    {
        return {ipart_sign * (ipart + fpart) * pow(10, exp)};
    }
#endif
}

static String string(Scanner &scan)
{
    StringBuilder builder{};

    scan.skip('"');

    while (scan.current() != '"' && scan.do_continue())
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

static Value array(Scanner &scan)
{
    scan.skip('[');

    Array array{};

    whitespace(scan);

    if (scan.skip(']'))
    {
        return move(array);
    }

    int index = 0;
    do
    {
        scan.skip(',');
        array.push_back(value(scan));
        index++;
    } while (scan.current() == ',');

    scan.skip(']');

    return move(array);
}

static Value object(Scanner &scan)
{
    scan.skip('{');

    Object object{};

    whitespace(scan);

    if (scan.skip('}'))
    {
        return move(object);
    }

    while (scan.current() != '}')
    {
        auto k = string(scan);
        whitespace(scan);

        scan.skip(':');

        object[k] = value(scan);

        scan.skip(',');

        whitespace(scan);
    }

    scan.skip('}');

    return object;
}

static Value keyword(Scanner &scan)
{
    StringBuilder builder{};

    while (scan.current_is(ALPHA) &&
           scan.do_continue())
    {
        builder.append(scan.current());
        scan.foreward();
    }

    auto keyword = builder.finalize();

    if (keyword == "true")
    {
        return true;
    }
    else if (keyword == "false")
    {
        return false;
    }
    else
    {
        return nullptr;
    }
}

static Value value(Scanner &scan)
{
    whitespace(scan);

    Value value{};

    if (scan.current() == '"')
    {
        value = string(scan);
    }
    else if (scan.current_is("-0123456789"))
    {
        value = number(scan);
    }
    else if (scan.current() == '{')
    {
        value = object(scan);
    }
    else if (scan.current() == '[')
    {
        value = array(scan);
    }
    else
    {
        value = keyword(scan);
    }

    whitespace(scan);

    return value;
}

Value parse(Scanner &scan)
{
    scan_skip_utf8bom(scan);
    return value(scan);
}

Value parse(const char *str, size_t size)
{
    StringScanner scan{str, size};
    return parse(scan);
};

Value parse_file(const char *path)
{
    __cleanup(stream_cleanup) Stream *json_file = stream_open(path, OPEN_READ);

    if (handle_has_error(json_file))
    {
        return nullptr;
    }

    StreamScanner scan{json_file};
    scan_skip_utf8bom(scan);
    return value(scan);
}

} // namespace json
