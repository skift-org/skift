#pragma once

#include <libio/Format.h>
#include <libio/Prettier.h>
#include <libjson/Value.h>
#include <libutils/String.h>

namespace Json
{

inline void prettify(IO::Prettier &pretty, const Value &value)
{
    if (value.is(STRING))
    {
        IO::write(pretty, '"');
        IO::write(pretty, value.as_string());
        IO::write(pretty, '"');
    }
    else if (value.is(INTEGER))
    {
        IO::Formatter::decimal().format(pretty, value.as_integer());
    }
#ifndef __KERNEL__
    else if (value.is(DOUBLE))
    {
        IO::Formatter::decimal().format(pretty, value.as_double());
    }
#endif
    else if (value.is(OBJECT))
    {
        IO::write(pretty, '{');

        if (value.length() > 0)
        {
            pretty.push_ident();

            auto object = value.as_object();

            size_t index = 0;

            object.foreach([&](auto &key, auto &value) {
                pretty.ident();

                pretty.color_depth();

                IO::write(pretty, '"');
                IO::write(pretty, key);
                IO::write(pretty, '"');

                pretty.color_clear();

                IO::write(pretty, ": ");
                prettify(pretty, value);

                if (index + 1 < object.count())
                {
                    IO::write(pretty, ',');
                }

                index++;
                return Iteration::CONTINUE;
            });

            pretty.pop_ident();
        }

        pretty.ident();
        IO::write(pretty, '}');
    }
    else if (value.is(ARRAY))
    {
        IO::write(pretty, '[');

        if (value.length() > 0)
        {
            pretty.push_ident();

            for (size_t i = 0; i < value.length(); i++)
            {
                pretty.ident();
                prettify(pretty, value.get(i));

                if (i + 1 < value.length())
                {
                    IO::write(pretty, ',');
                }
            }

            pretty.pop_ident();
        }

        pretty.ident();
        IO::write(pretty, ']');
    }
    else
    {
        IO::write(pretty, value.as_string());
    }
}

inline String stringify(const Value &value)
{
    IO::MemoryWriter memory;
    IO::Prettier pretty{memory};
    Json::prettify(pretty, value);
    return memory.string();
}

} // namespace Json
