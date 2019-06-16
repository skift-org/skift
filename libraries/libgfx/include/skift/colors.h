#pragma once

#include <skift/runtime.h>

typedef union {
    struct
    {
        ubyte R;
        ubyte G;
        ubyte B;
        ubyte A;
    };
    uint packed;
} color_t;

color_t RGB(float R, float G, float B);
color_t RGBA(float R, float G, float B, float A);
color_t HSV(float H, float S, float V);
color_t HSVA(float H, float S, float V, float A);

color_t color_blend(color_t fg, color_t bg);