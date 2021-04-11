#pragma once

#include <libgraphic/Icon.h>

namespace Widget
{

#define VARIANT_STRING_SIZE 128

enum VarianType
{
    INT,
    FLOAT,
    STRING,
};

class Variant
{
private:
    VarianType _type;

    RefPtr<Graphic::Icon> _icon = nullptr;
    int _as_int;
    float _as_float;
    char _as_string[VARIANT_STRING_SIZE];

public:
    bool has_icon()
    {
        return _icon != nullptr;
    }

    RefPtr<Graphic::Icon> icon() { return _icon; }

    VarianType type() { return _type; }

    int as_int() { return _as_int; }

    float as_float() { return _as_float; }

    const char *as_string() { return _as_string; }

    Variant(int value);

    Variant(float value);

    Variant(const char *fmt, ...);

    Variant(String str);

    Variant with_icon(RefPtr<Graphic::Icon> icon)
    {
        _icon = icon;
        return *this;
    }
};

int variant_cmp(Variant left, Variant right);

} // namespace Widget
