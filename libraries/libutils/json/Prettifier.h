#pragma once

#include <skift/NumberFormatter.h>

#include <libutils/Prettifier.h>
#include <libutils/Scanner.h>
#include <libutils/String.h>
#include <libutils/json/Value.h>

namespace json
{
inline void prettify(Prettifier &pretty, const Value &value)
{
    if (value.is(STRING))
    {
        pretty.append('"');
        pretty.append(value.as_string());
        pretty.append('"');
    }
    else if (value.is(INTEGER))
    {
        char buffer[128];
        format_int(FORMAT_DECIMAL, value.as_integer(), buffer, 128);
        pretty.append(buffer);
    }
#ifndef __KERNEL__
    else if (value.is(DOUBLE))
    {
        char buffer[128];
        format_double(FORMAT_DECIMAL, value.as_double(), buffer, 128);
        pretty.append(buffer);
    }
#endif
    else if (value.is(OBJECT))
    {
        pretty.append('{');

        if (value.length() > 0)
        {
            pretty.push_ident();

            value.as_object().foreach ([&](auto &key, auto &value) {
                pretty.ident();

                pretty.color_depth();

                pretty.append('"');
                pretty.append(key.cstring());
                pretty.append('"');

                pretty.color_clear();

                pretty.append(": ");
                prettify(pretty, value);
                pretty.append(',');

                return Iteration::CONTINUE;
            });

            pretty.rewind(1);

            pretty.pop_ident();
        }

        pretty.ident();
        pretty.append('}');
    }
    else if (value.is(ARRAY))
    {
        pretty.append('[');

        if (value.length() > 0)
        {
            pretty.push_ident();

            for (size_t i = 0; i < value.length(); i++)
            {
                pretty.ident();
                prettify(pretty, value.get(i));
                pretty.append(',');
            }

            pretty.rewind(1); // remove the last ","

            pretty.pop_ident();
        }

        pretty.ident();
        pretty.append(']');
    }
    else
    {
        pretty.append(value.as_string());
    }
}

} // namespace json
