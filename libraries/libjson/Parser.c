#include <libjson/Json.h>
#include <libmath/math.h>
#include <libsystem/cstring.h>
#include <libsystem/io/Stream.h>
#include <libsystem/unicode/Codepoint.h>
#include <libsystem/utils/BufferBuilder.h>
#include <libsystem/utils/NumberParser.h>

typedef struct
{
    const char *str;
    size_t size;
    size_t offset;
} JsonParser;

static JsonValue *value(JsonParser *parser);

static void next(JsonParser *parser)
{
    if (parser->offset < parser->size)
    {
        parser->offset++;
    }
}

static char current(JsonParser *parser)
{
    if (parser->offset < parser->size)
    {
        return parser->str[parser->offset];
    }
    else
    {
        return '\0';
    }
}

static bool current_is(JsonParser *parser, const char *what)
{
    for (size_t i = 0; what[i]; i++)
    {
        if (current(parser) == what[i])
        {
            return true;
        }
    }

    return false;
}

static void whitespace(JsonParser *parser)
{
    while (current_is(parser, " \n\r\t"))
    {
        next(parser);
    }
}

int digits(JsonParser *parser)
{
    int digits = 0;

    while (current_is(parser, "0123456789"))
    {
        digits *= 10;
        digits += current(parser) - '0';
        next(parser);
    }

    return digits;
}

static JsonValue *number(JsonParser *parser)
{
    int ipart_sign = 1;

    if (current(parser) == '-')
    {
        ipart_sign = -1;
        next(parser);
    }

    int ipart = 0;

    if (current_is(parser, "0123456789"))
    {
        ipart = digits(parser);
    }

    double fpart = 0;

    if (current(parser) == '.')
    {
        next(parser);

        double multiplier = 0.1;

        while (current_is(parser, "0123456789"))
        {
            fpart += multiplier * (current(parser) - '0');
            multiplier *= 0.1;
            next(parser);
        }
    }

    int exp = 0;

    if (current_is(parser, "eE"))
    {
        next(parser);
        int exp_sign = 1;

        if (current(parser) == '-')
        {
            exp_sign = -1;
        }

        if (current_is(parser, "+-"))
            next(parser);

        exp = digits(parser) * exp_sign;
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

static char *string(JsonParser *parser)
{
    BufferBuilder *builder = buffer_builder_create(16);

    if (current(parser) == '"')
        next(parser);

    while (current(parser) != '"')
    {
        if (current(parser) == '\\')
        {
            next(parser);
            switch (current(parser))
            {
            case '"':
                buffer_builder_append_chr(builder, '"');
                next(parser);

                break;

            case '\\':
                buffer_builder_append_chr(builder, '\\');
                next(parser);

                break;

            case '/':
                buffer_builder_append_chr(builder, '/');
                next(parser);

                break;

            case 'b':
                buffer_builder_append_chr(builder, '\b');
                next(parser);

                break;

            case 'f':
                buffer_builder_append_chr(builder, '\f');
                next(parser);

                break;

            case 'n':
                buffer_builder_append_chr(builder, '\n');
                next(parser);

                break;

            case 'r':
                buffer_builder_append_chr(builder, '\r');
                next(parser);

                break;

            case 't':
                buffer_builder_append_chr(builder, '\t');
                next(parser);

                break;

            case 'u':
            {
                char buffer[5];
                next(parser);
                for (size_t i = 0; i < 4 && current_is(parser, "0123456789abcdef"); i++)
                {
                    buffer[i] = current(parser);
                    next(parser);
                }

                _Static_assert(sizeof(Codepoint) == sizeof(unsigned int), "Expecting size of Codepoint being the same as unsigned int");

                Codepoint codepoint = 0;
                if (parse_uint(PARSER_HEXADECIMAL, buffer, 5, (unsigned int *)&codepoint))
                {
                    uint8_t utf8[5] = {};
                    codepoint_to_utf8(codepoint, utf8);
                    buffer_builder_append_str(builder, (char *)utf8);
                }

                break;
            }

            default:
                break;
            }
        }
        else
        {
            buffer_builder_append_chr(builder, current(parser));
            next(parser);
        }
    }

    if (current(parser) == '"')
        next(parser);

    return buffer_builder_finalize(builder);
}

static JsonValue *array(JsonParser *parser)
{
    next(parser); // skip [

    JsonValue *array = json_create_array();

    int index = 0;
    do
    {
        if (current(parser) == ',')
            next(parser);

        json_array_put(array, index, value(parser));
        index++;
    } while (current(parser) == ',');

    if (current(parser) == ']')
        next(parser); // skip ]

    return array;
}

static JsonValue *object(JsonParser *parser)
{
    next(parser); // skip {

    JsonValue *object = json_create_object();

    whitespace(parser);
    while (current(parser) != '}')
    {
        char *k = string(parser);
        whitespace(parser);

        if (current(parser) == ':')
        {
            next(parser);
        }

        JsonValue *v = value(parser);

        json_object_put(object, k, v);
        free(k);

        if (current(parser) == ',')
        {
            next(parser);
        }
        whitespace(parser);
    }

    if (current(parser) == '}')
        next(parser); // skip }

    return object;
}

static JsonValue *keyword(JsonParser *parser)
{
    BufferBuilder *builder = buffer_builder_create(6);

    while (current_is(parser, "abcdefghijklmnopqrstuvwxyz"))
    {
        buffer_builder_append_chr(builder, current(parser));
        next(parser);
    }

    char *keyword = buffer_builder_finalize(builder);

    if (strcmp(keyword, "true") == 0)
    {
        free(keyword);
        return json_create_true();
    }
    else if (strcmp(keyword, "false") == 0)
    {
        free(keyword);
        return json_create_false();
    }
    else
    {
        free(keyword);
        return json_create_null();
    }
}

static JsonValue *value(JsonParser *parser)
{
    whitespace(parser);

    JsonValue *value = NULL;

    if (current(parser) == '"')
    {
        value = json_create_string_adopt(string(parser));
    }
    else if (current_is(parser, "-0123456789"))
    {
        value = number(parser);
    }
    else if (current(parser) == '{')
    {
        value = object(parser);
    }
    else if (current(parser) == '[')
    {
        value = array(parser);
    }
    else
    {
        value = keyword(parser);
    }

    whitespace(parser);

    return value;
}

JsonValue *json_parse(const char *str, size_t size)
{
    JsonParser parser = {str, size, 0};

    return value(&parser);
}

JsonValue *json_parse_file(const char *path)
{
    Stream *json_file = stream_open(path, OPEN_READ);

    if (handle_has_error(json_file))
    {
        stream_close(json_file);
        return NULL;
    }

    FileState json_state = {};
    stream_stat(json_file, &json_state);

    if (handle_has_error(json_file))
    {
        stream_close(json_file);
        return NULL;
    }

    char *buffer = (char *)malloc(json_state.size);
    stream_read(json_file, buffer, json_state.size);

    if (handle_has_error(json_file))
    {
        free(buffer);
        stream_close(json_file);
        return NULL;
    }

    return json_parse(buffer, json_state.size);
}
