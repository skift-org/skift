#include <skift/colors.h>

color_t RGB(float R, float G, float B)
{
    return RGBA(R, G, B, 1.0);
}

color_t RGBA(float R, float G, float B, float A)
{
    color_t rgb;

    rgb.R = (int)(R * 255.0);
    rgb.G = (int)(G * 255.0);
    rgb.B = (int)(B * 255.0);
    rgb.A = (int)(A * 255.0);

    return rgb;
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