
#include <libgraphic/Color.h>
#include <libsystem/math/Lerp.h>

Color RGB(float R, float G, float B)
{
    return RGBA(R, G, B, 1.0);
}

Color HSV(float H, float S, float V)
{
    return HSVA(H, S, V, 1.0);
}

Color HSVA(float H, float S, float V, float A)
{
    if (S == 0)
    {
        return RGBA(V, V, V, A);
    }

    if (H == 360)
        H = 0;
    else
        H = H / 60;

    int i = (int)H;

    float f = H - i;
    float p = V * (1.0 - S);
    float q = V * (1.0 - (S * f));
    float t = V * (1.0 - (S * (1.0 - f)));

    switch (i)
    {
    case 0:
        return RGBA(V, t, p, A);

    case 1:
        return RGBA(q, V, p, A);

    case 2:
        return RGBA(p, V, t, A);

    case 3:
        return RGBA(p, q, V, A);

    case 4:
        return RGBA(t, p, V, A);

    default:
        return RGBA(V, p, q, A);
    }
}

Color ALPHA(Color color, float alpha)
{
    color.A = alpha * 255.0;

    return color;
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
    return color_lerp(color_lerp(c00, c10, transitionx),
                      color_lerp(c01, c11, transitionx), transitiony);
}

// TODO: color form name
// Color color_from_name(const char *name)
// {
// }
//
// void color_get_name(Color color, char *name, int size)
// {
// }
