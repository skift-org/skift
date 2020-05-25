#pragma once

#include <libgraphic/Bitmap.h>

#define VARIANT_STRING_SIZE 128

typedef enum
{
    VARIANT_INT,
    VARIANT_FLOAT,
    VARIANT_STRING,
} VarianType;

typedef struct
{
    VarianType type;

    Bitmap *icon;

    union {
        int as_int;
        float as_float;
    };

    char as_string[VARIANT_STRING_SIZE];
} Variant;

Variant vint(int value);

Variant vfloat(float value);

Variant vstring(const char *value);

Variant vstringf(const char *fmt, ...);

Variant variant_with_icon(Variant variant, const char *icon);

int variant_cmp(Variant left, Variant right);
