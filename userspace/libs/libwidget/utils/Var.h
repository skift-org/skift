#pragma once

#include <libgraphic/Icon.h>

namespace Widget
{

#define VAR_STRING_SIZE 128

enum VarType
{
    INT,
    FLOAT,
    STRING,
};

struct Var
{
private:
    VarType _type;

    Ref<Graphic::Icon> _icon = nullptr;
    int _as_int;
    float _as_float;
    char _as_string[VAR_STRING_SIZE];

public:
    bool has_icon()
    {
        return _icon != nullptr;
    }

    Ref<Graphic::Icon> icon() { return _icon; }

    VarType type() { return _type; }

    int as_int() { return _as_int; }

    float as_float() { return _as_float; }

    const char *as_string() { return _as_string; }

    Var(int value);

    Var(float value);

    Var(const char *fmt, ...);

    Var(String str);

    Var with_icon(Ref<Graphic::Icon> icon)
    {
        _icon = icon;
        return *this;
    }
};

int variant_cmp(Var left, Var right);

} // namespace Widget
