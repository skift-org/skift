#pragma once

#include <libgraphic/Bitmap.h>
#include <libgraphic/Color.h>
#include <libmath/Mat3x2.h>
#include <libsystem/Logger.h>
#include <libutils/Array.h>
#include <libutils/Variant.h>

namespace Graphic
{

static constexpr auto MAX_GRADIENT_STOPS = 16;

struct GradientStop
{
    Color color;
    float value;
};

struct Fill
{
    Color color = Colors::BLACK;
};

struct Gradient
{
    Optional<Math::Mat3x2f> transfom;
    Array<GradientStop, MAX_GRADIENT_STOPS> stops;
    size_t count;
};

struct Texture
{
    Optional<Math::Mat3x2f> transfom;
    RefPtr<Bitmap> bitmap;
};

using Paint = Utils::Variant<Fill, Gradient, Texture>;

static inline ALWAYS_INLINE Color sample(Paint &paint, Math::Vec2f p)
{
    Color result = Colors::CYAN;

    paint.visit(Utils::Visitor{
        [&](Fill &fill) {
            result = fill.color;
        },
        [&](Gradient &gradient) {
            if (gradient.transfom.present())
            {
                p = gradient.transfom.unwrap().apply(p);
            }

            float v = p.x();

            if (gradient.count == 0)
            {
                result = Colors::BLACK;
            }
            else if (gradient.count == 1 || v <= gradient.stops[0].value)
            {
                result = gradient.stops[0].color;
            }
            else if (v >= gradient.stops[gradient.count - 1].value)
            {
                result = gradient.stops[gradient.count - 1].color;
            }
            else
            {
                for (size_t i = 0; i + 1 < gradient.count; i++)
                {
                    GradientStop s0 = gradient.stops[i];
                    GradientStop s1 = gradient.stops[i + 1];

                    if (s0.value <= v && v < s1.value)
                    {
                        float localv = ((v - s0.value) / (s1.value - s0.value));
                        result = Color::lerp(s0.color, s1.color, localv);
                        break;
                    }
                }
            }
        },
        [&](Texture &texture) {
            if (texture.transfom.present())
            {
                p = texture.transfom.unwrap().apply(p);
            }

            result = texture.bitmap->sample(p);
        },
    });

    return result;
}

} // namespace Graphic
