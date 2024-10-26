#pragma once

#include <karm-gfx/color.h>

namespace Mixbox {

static constexpr auto LATENT_SIZE = 7;

using Latent = Array<f64, LATENT_SIZE>;

Latent unmix(Math::Vec3f rgb);

Math::Vec3f mix(Latent latent);

Latent unmixColor(Gfx::Color color);

Gfx::Color mixColor(Latent latent);

Math::Vec3f lerp(Math::Vec3f a, Math::Vec3f b, f64 t);

Gfx::Color lerpColor(Gfx::Color a, Gfx::Color b, f64 t);

} // namespace Mixbox
