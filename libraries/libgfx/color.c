#include <skift/color.h>
#include <skift/math.h>

color_t RGB(float R, float G, float B)
{
    return RGBA(R, G, B, 1.0);
}

color_t RGBA(float R, float G, float B, float A)
{
    return (color_t){{(int)(R * 255.0), (int)(G * 255.0), (int)(B * 255.0), (int)(A * 255.0)}};
}

color_t HSV(float H, float S, float V)
{
    return HSVA(H, S, V, 1.0);
}

color_t HSVA(float H, float S, float V, float A)
{
    float r = 0, g = 0, b = 0;

    if (S == 0)
    {
        r = V; g = V; b = V;
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

    color_t rgb;
    rgb.R = (int)(r * 255.0);
    rgb.G = (int)(g * 255.0);
    rgb.B = (int)(b * 255.0);
    rgb.A = (int)(A * 255.0);

    return rgb;
}

color_t color_blend(color_t fg, color_t bg)
{
    color_t result;

    uint alpha = fg.A + 1;
    uint inv_alpha = 256 - fg.A;

    result.R = (ubyte)((alpha * fg.R + inv_alpha * bg.R) / 256);
    result.G = (ubyte)((alpha * fg.G + inv_alpha * bg.G) / 256);
    result.B = (ubyte)((alpha * fg.B + inv_alpha * bg.B) / 256);
    result.A = 255;

    return result;
}

color_t color_lerp(color_t a, color_t b, float transition)
{
    color_t result;

    result.R = lerp(a.R, b.R, transition);
    result.G = lerp(a.G, b.G, transition);
    result.B = lerp(a.B, b.B, transition);
    result.A = lerp(a.A, b.A, transition);

    return result;
}

color_t color_blerp(color_t c00, color_t c10, color_t c01, color_t c11, float transitionx, float transitiony)
{
    return color_lerp(color_lerp(c00, c10, transitionx), color_lerp(c01, c11, transitionx), transitiony);
}

// TODO: color form name
// color_t color_from_name(const char *name)
// {
// }
// 
// void color_get_name(color_t color, char *name, int size)
// {
// }