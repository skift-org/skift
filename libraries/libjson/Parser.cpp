#include <libjson/Json.h>
#include <libsystem/Assert.h>
#include <libsystem/core/CString.h>
#include <libsystem/io/Stream.h>
#include <libsystem/math/Math.h>
#include <libsystem/unicode/Codepoint.h>
#include <libsystem/utils/BufferBuilder.h>
#include <libsystem/utils/Lexer.h>
#include <libsystem/utils/NumberParser.h>

#define JSON_WHITESPACE " \n\r\t"
#define JSON_DIGITS "0123456789"
#define JSON_ALPHA "abcdefghijklmnopqrstuvwxyz"

static JsonValue *value(Lexer &lexer);

static void whitespace(Lexer &lexer)
{
    lexer.eat(JSON_WHITESPACE);
}

static int digits(Lexer &lexer)
{
    int digits = 0;

    while (lexer.current_is(JSON_DIGITS))
    {
        digits *= 10;
        digits += lexer.current() - '0';
        lexer.foreward();
    }

    return digits;
}

static JsonValue *number(Lexer &lexer)
{
    int ipart_sign = 1;

    if (lexer.skip('-'))
    {
        ipart_sign = -1;
    }

    int ipart = 0;

    if (lexer.current_is(JSON_DIGITS))
    {
        ipart = digits(lexer);
    }

    double fpart = 0;

    if (lexer.current() == '.')
    {
        lexer.foreward();

        double multiplier = 0.1;

        while (lexer.current_is(JSON_DIGITS))
        {
            fpart += multiplier * (lexer.current() - '0');
            multiplier *= 0.1;
            lexer.foreward();
        }
    }

    int exp = 0;

    if (lexer.current_is("eE"))
    {
        lexer.foreward();
        int exp_sign = 1;

        if (lexer.current() == '-')
        {
            exp_sign = -1;
        }

        if (lexer.current_is("+-"))
            lexer.foreward();

        exp = digits(lexer) * exp_sign;
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

static char *string(Lexer &lexer)
{
    BufferBuilder *builder = buffer_builder_create(16);

    lexer.skip('"');

    while (lexer.current() != '"' &&
           lexer.do_continue())
    {
        if (lexer.current() == '\\')
        {
            buffer_builder_append_str(builder, lexer.read_escape_sequence());
        }
        else
        {
            buffer_builder_append_chr(builder, lexer.current());
            lexer.foreward();
        }
    }

    lexer.skip('"');

    return buffer_builder_finalize(builder);
}

static JsonValue *array(Lexer &lexer)
{
    lexer.skip('[');

    JsonValue *array = json_create_array();

    int index = 0;
    do
    {
        lexer.skip(',');

        json_array_put(array, index, value(lexer));
        index++;
    } while (lexer.current() == ',');

    lexer.skip(']');

    return array;
}

static JsonValue *object(Lexer &lexer)
{
    lexer.skip('{');

    JsonValue *object = json_create_object();

    whitespace(lexer);
    while (lexer.current() != '}')
    {
        char *k __cleanup_malloc = string(lexer);
        whitespace(lexer);

        lexer.skip(':');

        JsonValue *v = value(lexer);

        json_object_put(object, k, v);

        lexer.skip(',');

        whitespace(lexer);
    }

    lexer.skip('}');

    return object;
}

static JsonValue *keyword(Lexer &lexer)
{
    BufferBuilder *builder = buffer_builder_create(6);

    while (lexer.current_is(JSON_ALPHA) &&
           lexer.do_continue())
    {
        buffer_builder_append_chr(builder, lexer.current());
        lexer.foreward();
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

static JsonValue *value(Lexer &lexer)
{
    whitespace(lexer);

    JsonValue *value = nullptr;

    if (lexer.current() == '"')
    {
        value = json_create_string_adopt(string(lexer));
    }
    else if (lexer.current_is("-0123456789"))
    {
        value = number(lexer);
    }
    else if (lexer.current() == '{')
    {
        value = object(lexer);
    }
    else if (lexer.current() == '[')
    {
        value = array(lexer);
    }
    else
    {
        value = keyword(lexer);
    }

    whitespace(lexer);

    return value;
}

JsonValue *json_parse(const char *string, size_t size)
{
    Lexer lexer(string, size);
    // Skip the utf8 bom header if present.
    lexer.skip_word("\xEF\xBB\xBF");

    JsonValue *result = value(lexer);

    return result;
}

JsonValue *json_parse_file(const char *path)
{
    __cleanup(stream_cleanup) Stream *json_file = stream_open(path, OPEN_READ);

    if (handle_has_error(json_file))
    {
        return nullptr;
    }

    Lexer lexer(json_file);
    // Skip the utf8 bom header if present.
    lexer.skip_word("\xEF\xBB\xBF");

    JsonValue *result = value(lexer);

    return result;
}
