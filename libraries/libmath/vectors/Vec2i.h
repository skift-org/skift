#pragma once

typedef union
{
    struct
    {
        int x, y;
    };

    struct
    {
        int width, height;
    };

    int components[2];
} Vec2i;

#define vec2i(__x, __y) ((Vec2i){{(__x), (__y)}})