#include <libsystem/assert.h>
#include <libsystem/cstring.h>
#include <libwidget/Variant.h>

Variant vint(long int value)
{
    Variant v = (Variant){
        .type = VARIANT_INT,
        .as_int = value,
    };

    snprintf(v.as_string, VARIANT_STRING_SIZE, "%ld", value);

    return v;
}

Variant vfloat(float value)
{
    Variant v = (Variant){
        .type = VARIANT_FLOAT,
        .as_float = value,
    };

    snprintf(v.as_string, VARIANT_STRING_SIZE, "%.2f", value);

    return v;
}

Variant vstring(const char *value)
{
    Variant v = (Variant){.type = VARIANT_STRING};

    assert(strlen(value) < VARIANT_STRING_SIZE);

    strncpy(v.as_string, value, VARIANT_STRING_SIZE);

    return v;
}

Variant vstringf(const char *fmt, ...)
{
    Variant v = (Variant){.type = VARIANT_STRING};

    va_list args;
    va_start(args, fmt);
    vsnprintf(v.as_string, VARIANT_STRING_SIZE, fmt, args);
    va_end(args);

    return v;
}

int variant_cmp(Variant left, Variant right)
{
    if (left.type == VARIANT_INT && right.type == VARIANT_INT)
    {
        return left.as_int - right.as_int;
    }
    else if (left.type == VARIANT_FLOAT && right.type == VARIANT_FLOAT)
    {
        return left.as_float - right.as_float;
    }
    else
    {
        return strcmp(left.as_string, right.as_string);
    }
}