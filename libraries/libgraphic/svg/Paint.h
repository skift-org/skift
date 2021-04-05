#pragma once

#include <libgraphic/Bitmap.h>
#include <libgraphic/Color.h>
#include <libsystem/Logger.h>
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
    GradiantStop stops[MAX_GRADIANT_STOPS];
    size_t count;
};

struct Texture
{
    Trans2f transfom;
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
            __unused(gradiant);
            // TODO: Gradient paint
            result = Colors::MAGENTA;
        },
        [&](Texture &texture) {
            p = texture.transfom.apply(p);
            result = texture.bitmap->sample(p);
        },
    });

    return result;
}

} // namespace Graphic
