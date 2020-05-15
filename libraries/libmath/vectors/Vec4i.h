#pragma once

typedef union
{
    struct
    {
        int x, y, z, w;
    };

    int components[4];
} Vec4i;

#define vec4i(__x, __y, __z, __w) ((Vec4i){{(__x), (__y), (__z), (__w)}})
