#include <assert.h>
#include <libwidget/utils/Variant.h>
#include <stdio.h>
#include <string.h>

namespace Widget
{

Variant::Variant(int value) : _type(VarianType::INT)
{
    _as_float = value;
    _as_int = value;
    snprintf(_as_string, VARIANT_STRING_SIZE, "%d", value);
}

Variant::Variant(float value) : _type(VarianType::FLOAT)
{
    _as_float = value;
    _as_int = value;
    snprintf(_as_string, VARIANT_STRING_SIZE, "%.2f", value);
}

Variant::Variant(const char *fmt, ...) : _type(VarianType::STRING)
{
    assert(strlen(fmt) < VARIANT_STRING_SIZE);

    va_list args;
    va_start(args, fmt);
    vsnprintf(_as_string, VARIANT_STRING_SIZE, fmt, args);
    va_end(args);
}

Variant::Variant(String str) : Variant(str.cstring())
{
}

int variant_cmp(Variant left, Variant right)
{
    if (left.type() == VarianType::INT && right.type() == VarianType::INT)
    {
        return left.as_int() - right.as_int();
    }
    else if (left.type() == VarianType::FLOAT && right.type() == VarianType::FLOAT)
    {
        return left.as_float() - right.as_float();
    }
    else
    {
        return strcmp(left.as_string(), right.as_string());
    }
}

} // namespace Widget
