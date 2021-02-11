#pragma once

#include <libutils/NumberParser.h>
#include <libutils/Scanner.h>
#include <libutils/ScannerUtils.h>
#include <libutils/StringBuilder.h>
#include <libutils/Strings.h>
#include <libutils/json/Value.h>

namespace json
{
Value value(Scanner &scan);

inline void whitespace(Scanner &scan)
{
    scan.eat(Strings::WHITESPACE);
}

inline Value number(Scanner &scan)
{
#ifdef __KERNEL__
    return Value{scan_int(scan, 10)};
#else
    return {scan_float(scan)};
#endif
}

inline String string(Scanner &scan)
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

inline Value array(Scanner &scan)
{
    scan.skip('[');

    Value::Array array{};

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

inline Value object(Scanner &scan)
{
    scan.skip('{');

    Value::Object object{};

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

inline Value keyword(Scanner &scan)
{
    StringBuilder builder{};

    while (scan.current_is(Strings::LOWERCASE_ALPHA) &&
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

inline Value value(Scanner &scan)
{
    whitespace(scan);

    Value value{};

    if (scan.current() == '"')
    {
        value = string(scan);
    }
    else if (scan.current_is("-") ||
             scan.current_is("0123456789"))
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

inline Value parse(Scanner &scan)
{
    scan_skip_utf8bom(scan);
    return value(scan);
}

inline Value parse(const char *str, size_t size)
{
    StringScanner scan{str, size};
    return parse(scan);
};

inline Value parse(const String &str)
{
    StringScanner scan{str.cstring(), str.length()};
    return parse(scan);
};

inline Value parse_file(String path)
{
    __cleanup(stream_cleanup) Stream *json_file = stream_open(path.cstring(), OPEN_READ | OPEN_BUFFERED);

    if (handle_has_error(json_file))
    {
        return nullptr;
    }

    StreamScanner scan{json_file};
    scan_skip_utf8bom(scan);
    return value(scan);
}

} // namespace json