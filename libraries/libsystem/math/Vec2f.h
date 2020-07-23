#pragma once

typedef union {
    struct
    {
        float x, y;
    };

    struct
    {
        float width, height;
    };

    float components[2];
} Vec2f;

#define vec2f(__x, __y) ((Vec2f){{(float)(__x), (float)(__y)}})
