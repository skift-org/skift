#include <libjson/Json.h>
#include <libsystem/Assert.h>
#include <libsystem/core/CString.h>
#include <libsystem/io/Stream.h>
#include <libsystem/math/Math.h>
#include <libsystem/unicode/Codepoint.h>
#include <libsystem/utils/BufferBuilder.h>
#include <libsystem/utils/Lexer.h>
#include <libsystem/utils/NumberParser.h>

namespace json
{
static constexpr const char *WHITESPACE = " \n\r\t";
static constexpr const char *DIGITS = "0123456789";
static constexpr const char *ALPHA = "abcdefghijklmnopqrstuvwxyz";

static Value *value(Lexer &lexer);

static void whitespace(Lexer &lexer)
{
    lexer.eat(WHITESPACE);
}

static int digits(Lexer &lexer)
{
    int digits = 0;

    while (lexer.current_is(DIGITS))
    {
        digits *= 10;
        digits += lexer.current() - '0';
        lexer.foreward();
    }

    return digits;
}

static Value *number(Lexer &lexer)
{
    int ipart_sign = 1;

    if (lexer.skip('-'))
    {
        ipart_sign = -1;
    }

    int ipart = 0;

    if (lexer.current_is(DIGITS))
    {
        ipart = digits(lexer);
    }

#ifdef __KERNEL__
    return create_integer(ipart_sign * ipart);
#else
    double fpart = 0;

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
        return create_integer(ipart_sign * ipart * pow(10, exp));
    }
    else
    {
        return create_double(ipart_sign * (ipart + fpart) * pow(10, exp));
    }
#endif
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

static Value *array(Lexer &lexer)
{
    lexer.skip('[');

    Value *array = create_array();

    int index = 0;
    do
    {
        lexer.skip(',');
        array_put(array, index, value(lexer));
        index++;
    } while (lexer.current() == ',');

    lexer.skip(']');

    return array;
}

static Value *object(Lexer &lexer)
{
    lexer.skip('{');

    Value *object = create_object();

    whitespace(lexer);
    while (lexer.current() != '}')
    {
        char *k __cleanup_malloc = string(lexer);
        whitespace(lexer);

        lexer.skip(':');

        Value *v = value(lexer);

        object_put(object, k, v);

        lexer.skip(',');

        whitespace(lexer);
    }

    lexer.skip('}');

    return object;
}

static Value *keyword(Lexer &lexer)
{
    BufferBuilder *builder = buffer_builder_create(6);

    while (lexer.current_is(ALPHA) &&
           lexer.do_continue())
    {
        buffer_builder_append_chr(builder, lexer.current());
        lexer.foreward();
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

static Value *value(Lexer &lexer)
{
    whitespace(lexer);

    Value *value = nullptr;

    if (lexer.current() == '"')
    {
        value = create_string_adopt(string(lexer));
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

Value *parse(const char *string, size_t size)
{
    Lexer lexer(string, size);
    // Skip the utf8 bom header if present.
    lexer.skip_word("\xEF\xBB\xBF");

    Value *result = value(lexer);

    return result;
}

Value *parse_file(const char *path)
{
    __cleanup(stream_cleanup) Stream *json_file = stream_open(path, OPEN_READ);

    if (handle_has_error(json_file))
    {
        return nullptr;
    }

    Lexer lexer(json_file);
    // Skip the utf8 bom header if present.
    lexer.skip_word("\xEF\xBB\xBF");

    Value *result = value(lexer);

    return result;
}
} // namespace json
