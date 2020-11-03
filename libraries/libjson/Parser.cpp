#include <libjson/Json.h>
#include <libsystem/Assert.h>
#include <libsystem/core/CString.h>
#include <libsystem/io/Stream.h>
#include <libsystem/math/Math.h>
#include <libsystem/unicode/Codepoint.h>
#include <libsystem/utils/BufferBuilder.h>
#include <libsystem/utils/NumberParser.h>
#include <libutils/Scanner.h>
#include <libutils/ScannerUtils.h>

namespace json
{
static constexpr const char *WHITESPACE = " \n\r\t";
static constexpr const char *DIGITS = "0123456789";
static constexpr const char *ALPHA = "abcdefghijklmnopqrstuvwxyz";

static Value *value(Scanner &scan);

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

static Value *number(Scanner &scan)
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
    return create_integer(ipart_sign * ipart);
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
        return create_integer(ipart_sign * ipart * pow(10, exp));
    }
    else
    {
        return create_double(ipart_sign * (ipart + fpart) * pow(10, exp));
    }
#endif
}

static char *string(Scanner &scan)
{
    BufferBuilder *builder = buffer_builder_create(16);

    scan.skip('"');

    while (scan.current() != '"' &&
           scan.do_continue())
    {
        if (scan.current() == '\\')
        {
            buffer_builder_append_str(builder, scan_json_escape_sequence(scan));
        }
        else
        {
            buffer_builder_append_chr(builder, scan.current());
            scan.foreward();
        }
    }

    scan.skip('"');

    return buffer_builder_finalize(builder);
}

static Value *array(Scanner &scan)
{
    scan.skip('[');

    Value *array = create_array();

    int index = 0;
    do
    {
        scan.skip(',');
        array_put(array, index, value(scan));
        index++;
    } while (scan.current() == ',');

    scan.skip(']');

    return array;
}

static Value *object(Scanner &scan)
{
    scan.skip('{');

    Value *object = create_object();

    whitespace(scan);
    while (scan.current() != '}')
    {
        char *k __cleanup_malloc = string(scan);
        whitespace(scan);

        scan.skip(':');

        Value *v = value(scan);

        object_put(object, k, v);

        scan.skip(',');

        whitespace(scan);
    }

    scan.skip('}');

    return object;
}

static Value *keyword(Scanner &scan)
{
    BufferBuilder *builder = buffer_builder_create(6);

    while (scan.current_is(ALPHA) &&
           scan.do_continue())
    {
        buffer_builder_append_chr(builder, scan.current());
        scan.foreward();
    }

    __cleanup_malloc char *keyword = buffer_builder_finalize(builder);

    if (strcmp(keyword, "true") == 0)
    {
        return create_boolean(true);
    }
    else if (strcmp(keyword, "false") == 0)
    {
        return create_boolean(false);
    }
    else
    {
        return create_nil();
    }
}

static Value *value(Scanner &scan)
{
    whitespace(scan);

    Value *value = nullptr;

    if (scan.current() == '"')
    {
        value = create_string_adopt(string(scan));
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

Value *parse(const char *string, size_t size)
{
    StringScanner scan{string, size};
    // Skip the utf8 bom header if present.
    scan.skip_word("\xEF\xBB\xBF");

    Value *result = value(scan);

    return result;
}

Value *parse_file(const char *path)
{
    __cleanup(stream_cleanup) Stream *json_file = stream_open(path, OPEN_READ);

    if (handle_has_error(json_file))
    {
        return nullptr;
    }

    StreamScanner scan{json_file};
    // Skip the utf8 bom header if present.
    scan.skip_word("\xEF\xBB\xBF");

    Value *result = value(scan);

    return result;
}
} // namespace json
