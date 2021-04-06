#pragma once

#include <libgraphic/Bitmap.h>
#include <libgraphic/Color.h>
#include <libsystem/Logger.h>
#include <libutils/Array.h>
#include <libutils/Trans2.h>
#include <libutils/Variant.h>

namespace Graphic
{

static constexpr auto MAX_GRADIANT_STOPS = 16;

struct GradiantStop
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
    Trans2f transfom;
    Array<GradiantStop, MAX_GRADIANT_STOPS> stops;
    size_t count;
};

struct Texture
{
    Optional<Trans2f> transfom;
    RefPtr<Bitmap> bitmap;
};

using Paint = Utils::Variant<Fill, Gradient, Texture>;

static inline ALWAYS_INLINE Color sample(Paint &paint, Vec2f p)
{
    Color result = Colors::CYAN;

    paint.visit(Utils::Visitor{
        [&](Fill &fill) {
            result = fill.color;
        },
        [&](Gradient &gradiant) {
            if (gradiant.count == 0)
            {
                result = Colors::BLACK;
            }
            else if (gradiant.count == 1)
            {
                result = gradiant.stops[0].color;
            }
            else
            {
                GradiantStop s0 = gradiant.stops[0];
                GradiantStop s1 = gradiant.stops[1];

                Color blend = Color::lerp(s0.color, s1.color, ((p.x() - s0.value) / (s1.value - s0.value)));

                result = blend;
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
