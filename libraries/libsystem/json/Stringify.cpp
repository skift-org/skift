#include <libsystem/json/Json.h>
#include <libsystem/utils/NumberFormatter.h>
#include <libutils/StringBuilder.h>

namespace json
{
struct PrettifyState
{
    StringBuilder builder{};
    int depth = 0;
    bool color = true;
    bool ident = true;
};

static const char *depth_color[] = {
    "\e[91m",
    "\e[92m",
    "\e[93m",
    "\e[94m",
    "\e[95m",
    "\e[96m",
};

static void prettify_internal(PrettifyState &state, const Value &value);

static void prettify_ident(PrettifyState &state)
{
    if (state.ident)
    {
        state.builder.append('\n');

        for (int i = 0; i < state.depth; i++)
        {
            state.builder.append("    ");
        }
    }
}

static Iteration prettify_object(PrettifyState &state, String &key, const Value &value)
{
    prettify_ident(state);

    if (state.color)
    {
        state.builder.append(depth_color[state.depth % 6]);
    }

    state.builder.append("\"");
    state.builder.append(key.cstring());
    state.builder.append("\"");

    if (state.color)
    {
        state.builder.append("\e[m");
    }

    state.builder.append(": ");
    prettify_internal(state, value);
    state.builder.append(',');

    return Iteration::CONTINUE;
}

static Iteration prettify_array(PrettifyState &state, const Value &value)
{
    prettify_ident(state);
    prettify_internal(state, value);
    state.builder.append(',');

    return Iteration::CONTINUE;
}

static void prettify_internal(PrettifyState &state, const Value &value)
{
    if (value.is(STRING))
    {
        state.builder.append('"');
        state.builder.append(value.as_string());
        state.builder.append('"');
    }
    else if (value.is(INTEGER))
    {
        char buffer[128];
        format_int(FORMAT_DECIMAL, value.as_integer(), buffer, 128);
        state.builder.append(buffer);
    }
#ifndef __KERNEL__
    else if (value.is(DOUBLE))
    {
        char buffer[128];
        format_double(FORMAT_DECIMAL, value.as_double(), buffer, 128);
        state.builder.append(buffer);
    }
#endif
    else if (value.is(OBJECT))
    {
        state.builder.append('{');

        if (value.length() > 0)
        {
            state.depth++;

            value.as_object().foreach ([&](auto &key, auto &value) {
                prettify_object(state, key, value);
                return Iteration::CONTINUE;
            });

            state.builder.rewind(1);
            state.depth--;
        }

        prettify_ident(state);
        state.builder.append('}');
    }
    else if (value.is(ARRAY))
    {
        state.builder.append('[');

        if (value.length() > 0)
        {
            state.depth++;

            for (size_t i = 0; i < value.length(); i++)
            {
                prettify_array(state, value.get(i));
            }

            state.builder.rewind(1); // remove the last ","

            state.depth--;
        }

        prettify_ident(state);

        state.builder.append(']');
    }
    else
    {
        state.builder.append(value.as_string());
    }
}

String prettify(const Value &value)
{
    PrettifyState state{};

    prettify_internal(state, value);

    return state.builder.finalize();
}

String stringify(const Value &value)
{
    PrettifyState state{};

    state.color = false;
    state.ident = false;

    prettify_internal(state, value);

    return state.builder.finalize();
}

} // namespace json
