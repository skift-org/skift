#include <libjson/Json.h>
#include <libmath/math.h>
#include <libsystem/Assert.h>
#include <libsystem/CString.h>
#include <libsystem/io/Stream.h>
#include <libsystem/unicode/Codepoint.h>
#include <libsystem/utils/BufferBuilder.h>
#include <libsystem/utils/NumberParser.h>
#include <libsystem/utils/SourceReader.h>

#define JSON_WHITESPACE " \n\r\t"
#define JSON_DIGITS "0123456789"
#define JSON_XDIGITS "0123456789abcdef"
#define JSON_ALPHA "abcdefghijklmnopqrstuvwxyz"

static JsonValue *value(SourceReader *source);

static void whitespace(SourceReader *source)
{
    source_eat(source, JSON_WHITESPACE);
}

static int digits(SourceReader *source)
{
    int digits = 0;

    while (source_current_is(source, JSON_DIGITS))
    {
        digits *= 10;
        digits += source_current(source) - '0';
        source_foreward(source);
    }

    return digits;
}

static JsonValue *number(SourceReader *source)
{
    int ipart_sign = 1;

    if (source_skip(source, '-'))
    {
        ipart_sign = -1;
    }

    int ipart = 0;

    if (source_current_is(source, JSON_DIGITS))
    {
        ipart = digits(source);
    }

    double fpart = 0;

    if (source_current(source) == '.')
    {
        source_foreward(source);

        double multiplier = 0.1;

        while (source_current_is(source, JSON_DIGITS))
        {
            fpart += multiplier * (source_current(source) - '0');
            multiplier *= 0.1;
            source_foreward(source);
        }
    }

    int exp = 0;

    if (source_current_is(source, "eE"))
    {
        source_foreward(source);
        int exp_sign = 1;

        if (source_current(source) == '-')
        {
            exp_sign = -1;
        }

        if (source_current_is(source, "+-"))
            source_foreward(source);

        exp = digits(source) * exp_sign;
    }

    if (fpart == 0 && exp >= 0)
    {
        return json_create_integer(ipart_sign * ipart * pow(10, exp));
    }
    else
    {
        return json_create_double(ipart_sign * (ipart + fpart) * pow(10, exp));
    }
}

static const char *escape(SourceReader *source)
{
    source_skip(source, '\\');
    char chr = source_current(source);

    switch (chr)
    {
    case '"':
        return "\"";

    case '\\':
        return "\\";

    case '/':
        return "/";

    case 'b':
        return "\b";

    case 'f':
        return "\f";

    case 'n':
        return "\n";

    case 'r':
        return "\r";

    case 't':
        return "\t";

    case 'u':
    {
        char buffer[5];
        for (size_t i = 0; i < 4 && source_current_is(source, JSON_XDIGITS); i++)
        {
            buffer[i] = source_current(source);
            source_foreward(source);
        }

        static_assert(sizeof(Codepoint) == sizeof(unsigned int), "Expecting size of Codepoint being the same as unsigned int");

        Codepoint codepoint = 0;
        if (parse_uint(PARSER_HEXADECIMAL, buffer, 5, (unsigned int *)&codepoint))
        {
            static uint8_t utf8[5] = {};
            codepoint_to_utf8(codepoint, utf8);

            return (char *)utf8;
        }
        else
        {
            return "\\uXXXX";
        }
    }

    default:
        break;
    }

    static char buffer[3] = {'\\', 'x', '\0'};
    buffer[1] = chr;

    return buffer;
}

static char *string(SourceReader *source)
{
    BufferBuilder *builder = buffer_builder_create(16);

    source_skip(source, '"');

    while (source_current(source) != '"' &&
           source_do_continue(source))
    {
        if (source_current(source) == '\\')
        {
            buffer_builder_append_str(builder, escape(source));
        }
        else
        {
            buffer_builder_append_chr(builder, source_current(source));
            source_foreward(source);
        }
    }

    source_skip(source, '"');

    return buffer_builder_finalize(builder);
}

static JsonValue *array(SourceReader *source)
{
    source_skip(source, '[');

    JsonValue *array = json_create_array();

    int index = 0;
    do
    {
        source_skip(source, ',');

        json_array_put(array, index, value(source));
        index++;
    } while (source_current(source) == ',');

    source_skip(source, ']');

    return array;
}

static JsonValue *object(SourceReader *source)
{
    source_skip(source, '{');

    JsonValue *object = json_create_object();

    whitespace(source);
    while (source_current(source) != '}')
    {
        char *k __cleanup_malloc = string(source);
        whitespace(source);

        source_skip(source, ':');

        JsonValue *v = value(source);

        json_object_put(object, k, v);

        source_skip(source, ',');

        whitespace(source);
    }

    source_skip(source, '}');

    return object;
}

static JsonValue *keyword(SourceReader *source)
{
    BufferBuilder *builder = buffer_builder_create(6);

    while (source_current_is(source, JSON_ALPHA) &&
           source_do_continue(source))
    {
        buffer_builder_append_chr(builder, source_current(source));
        source_foreward(source);
    }

    __cleanup_malloc char *keyword = buffer_builder_finalize(builder);

    if (strcmp(keyword, "true") == 0)
    {
        return json_create_boolean(true);
    }
    else if (strcmp(keyword, "false") == 0)
    {
        return json_create_boolean(false);
    }
    else
    {
        return json_create_null();
    }
}

static JsonValue *value(SourceReader *source)
{
    whitespace(source);

    JsonValue *value = NULL;

    if (source_current(source) == '"')
    {
        value = json_create_string_adopt(string(source));
    }
    else if (source_current_is(source, "-0123456789"))
    {
        value = number(source);
    }
    else if (source_current(source) == '{')
    {
        value = object(source);
    }
    else if (source_current(source) == '[')
    {
        value = array(source);
    }
    else
    {
        value = keyword(source);
    }

    whitespace(source);

    return value;
}

JsonValue *json_parse(const char *string, size_t size)
{
    SourceReader *source = source_create_from_string(string, size);

    JsonValue *result = value(source);

    source_destroy(source);

    return result;
}

JsonValue *json_parse_file(const char *path)
{
    __cleanup(stream_cleanup) Stream *json_file = stream_open(path, OPEN_READ);

    if (handle_has_error(json_file))
    {
        return NULL;
    }

    SourceReader *source = source_create_from_stream(json_file);

    JsonValue *result = value(source);

    source_destroy(source);

    return result;
}
