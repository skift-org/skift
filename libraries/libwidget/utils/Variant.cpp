#include <libsystem/Assert.h>
#include <libsystem/core/CString.h>
#include <libwidget/utils/Variant.h>

Variant vint(int value)
{
    Variant v = (Variant){
        .type = VARIANT_INT,
        .icon = NULL,
        .as_int = value,
        .as_string = {},
    };

    snprintf(v.as_string, VARIANT_STRING_SIZE, "%d", value);

    return v;
}

Variant vfloat(float value)
{
    Variant v = (Variant){
        .type = VARIANT_FLOAT,
        .icon = NULL,
        .as_float = value,
        .as_string = {},
    };

    snprintf(v.as_string, VARIANT_STRING_SIZE, "%.2f", value);

    return v;
}

Variant vstring(const char *value)
{
    Variant v = (Variant){
        .type = VARIANT_STRING,
        .icon = NULL,
        .as_int = -1,
        .as_string = {},
    };

    assert(strlen(value) < VARIANT_STRING_SIZE);

    strncpy(v.as_string, value, VARIANT_STRING_SIZE);

    return v;
}

Variant vstringf(const char *fmt, ...)
{
    Variant v = (Variant){
        .type = VARIANT_STRING,
        .icon = NULL,
        .as_int = -1,
        .as_string = {},
    };

    va_list args;
    va_start(args, fmt);
    vsnprintf(v.as_string, VARIANT_STRING_SIZE, fmt, args);
    va_end(args);

    return v;
}

Variant variant_with_icon(Variant variant, Icon *icon)
{
    variant.icon = icon;
    return variant;
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
