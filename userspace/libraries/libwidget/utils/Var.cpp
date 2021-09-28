#include <assert.h>
#include <stdio.h>
#include <string.h>

#include <libutils/Assert.h>
#include <libwidget/utils/Var.h>

namespace Widget
{

Var::Var(int value) : _type(VarType::INT)
{
    _as_float = value;
    _as_int = value;
    snprintf(_as_string, VAR_STRING_SIZE, "%d", value);
}

Var::Var(float value) : _type(VarType::FLOAT)
{
    _as_float = value;
    _as_int = value;
    snprintf(_as_string, VAR_STRING_SIZE, "%.2f", value);
}

Var::Var(const char *fmt, ...) : _type(VarType::STRING)
{
    Assert::lower_than(strlen(fmt), VAR_STRING_SIZE);

    va_list args;
    va_start(args, fmt);
    vsnprintf(_as_string, VAR_STRING_SIZE, fmt, args);
    va_end(args);
}

Var::Var(String str) : Var(str.cstring())
{
}

int variant_cmp(Var left, Var right)
{
    if (left.type() == VarType::INT && right.type() == VarType::INT)
    {
        return left.as_int() - right.as_int();
    }
    else if (left.type() == VarType::FLOAT && right.type() == VarType::FLOAT)
    {
        return left.as_float() - right.as_float();
    }
    else
    {
        return strcmp(left.as_string(), right.as_string());
    }
}

} // namespace Widget
