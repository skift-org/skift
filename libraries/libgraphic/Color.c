/* Copyright © 2018-2020 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <libgraphic/Color.h>
#include <libmath/Lerp.h>

Color RGB(float R, float G, float B)
{
    return RGBA(R, G, B, 1.0);
}

Color RGBA(float R, float G, float B, float A)
{
    return (Color){{(int)(R * 255.0), (int)(G * 255.0), (int)(B * 255.0), (int)(A * 255.0)}};
}

Color HSV(float H, float S, float V)
{
    return HSVA(H, S, V, 1.0);
}

Color HSVA(float H, float S, float V, float A)
{
    float r = 0, g = 0, b = 0;

    if (S == 0)
    {
        r = V;
        g = V;
        b = V;
    }
    else
    {
        int i;
        float f, p, q, t;

        if (H == 360)
            H = 0;
        else
            H = H / 60;

        i = (int)H;
        f = H - i;

        p = V * (1.0 - S);
        q = V * (1.0 - (S * f));
        t = V * (1.0 - (S * (1.0 - f)));

        switch (i)
        {
        case 0:
            r = V;
            g = t;
            b = p;
            break;

        case 1:
            r = q;
            g = V;
            b = p;
            break;

        case 2:
            r = p;
            g = V;
            b = t;
            break;

        case 3:
            r = p;
            g = q;
            b = V;
            break;

        case 4:
            r = t;
            g = p;
            b = V;
            break;

        default:
            r = V;
            g = p;
            b = q;
            break;
        }
    }

    Color rgb;
    rgb.R = (int)(r * 255.0);
    rgb.G = (int)(g * 255.0);
    rgb.B = (int)(b * 255.0);
    rgb.A = (int)(A * 255.0);

    return rgb;
}

Color ALPHA(Color color, float alpha)
{
    color.A = alpha * 255.0;

    return color;
}

Color color_blend(Color fg, Color bg)
{
    Color result;

    uint alpha = fg.A + 1;
    uint inv_alpha = 256 - fg.A;

    result.R = (ubyte)((alpha * fg.R + inv_alpha * bg.R) / 256);
    result.G = (ubyte)((alpha * fg.G + inv_alpha * bg.G) / 256);
    result.B = (ubyte)((alpha * fg.B + inv_alpha * bg.B) / 256);
    result.A = 255;

    return result;
}

Color color_lerp(Color a, Color b, float transition)
{
    Color result;

    result.R = lerp(a.R, b.R, transition);
    result.G = lerp(a.G, b.G, transition);
    result.B = lerp(a.B, b.B, transition);
    result.A = lerp(a.A, b.A, transition);

    return result;
}

Color color_blerp(Color c00, Color c10, Color c01, Color c11, float transitionx, float transitiony)
{
    return color_lerp(color_lerp(c00, c10, transitionx), color_lerp(c01, c11, transitionx), transitiony);
}

// TODO: color form name
// Color color_from_name(const char *name)
// {
// }
//
// void color_get_name(Color color, char *name, int size)
// {
// }