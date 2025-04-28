#include <karm-math/funcs.h>

#include "mixbox.h"

namespace Mixbox {

static u8 const _LUT[] = {
#include "mixbox.inc"
};

static f64 srgb2linear(f64 srgb) {
    if (srgb <= 0.04045f)
        return srgb / 12.92;
    return Math::pow((srgb + 0.055) / 1.055, 2.4);
}

static Math::Vec3f srgb2linear(Gfx::Color color) {
    return {
        srgb2linear(color.red / 255.0),
        srgb2linear(color.green / 255.0),
        srgb2linear(color.blue / 255.0),
    };
}

static f64 linear2srgb(f64 linear) {
    if (linear <= 0.0031308)
        return linear * 12.92;
    return 1.055 * Math::pow(linear, 1.0 / 2.4) - 0.055;
}

static Gfx::Color linear2srgb(Math::Vec3f linear) {
    return {
        static_cast<u8>(linear2srgb(linear[0]) * 255.0),
        static_cast<u8>(linear2srgb(linear[1]) * 255.0),
        static_cast<u8>(linear2srgb(linear[2]) * 255.0),
        255,
    };
}

static Math::Vec3f evalPolynomial(f64 c0, f64 c1, f64 c2, f64 c3) {
    auto c00 = c0 * c0;
    auto c11 = c1 * c1;
    auto c22 = c2 * c2;
    auto c33 = c3 * c3;
    auto c01 = c0 * c1;
    auto c02 = c0 * c2;
    auto c12 = c1 * c2;

    auto r = 0.0, g = 0.0, b = 0.0;

    // clang-format off
    auto w00 = c0 * c00; r +=  0.07717053*w00; g +=  0.02826978*w00; b +=  0.24832992*w00;
    auto w01 = c1 * c11; r +=  0.95912302*w01; g +=  0.80256528*w01; b +=  0.03561839*w01;
    auto w02 = c2 * c22; r +=  0.74683774*w02; g +=  0.04868586*w02; b +=  0.00000000*w02;
    auto w03 = c3 * c33; r +=  0.99518138*w03; g +=  0.99978149*w03; b +=  0.99704802*w03;
    auto w04 = c00 * c1; r +=  0.04819146*w04; g +=  0.83363781*w04; b +=  0.32515377*w04;
    auto w05 = c01 * c1; r += -0.68146950*w05; g +=  1.46107803*w05; b +=  1.06980936*w05;
    auto w06 = c00 * c2; r +=  0.27058419*w06; g += -0.15324870*w06; b +=  1.98735057*w06;
    auto w07 = c02 * c2; r +=  0.80478189*w07; g +=  0.67093710*w07; b +=  0.18424500*w07;
    auto w08 = c00 * c3; r += -0.35031003*w08; g +=  1.37855826*w08; b +=  3.68865000*w08;
    auto w09 = c0 * c33; r +=  1.05128046*w09; g +=  1.97815239*w09; b +=  2.82989073*w09;
    auto w10 = c11 * c2; r +=  3.21607125*w10; g +=  0.81270228*w10; b +=  1.03384539*w10;
    auto w11 = c1 * c22; r +=  2.78893374*w11; g +=  0.41565549*w11; b += -0.04487295*w11;
    auto w12 = c11 * c3; r +=  3.02162577*w12; g +=  2.55374103*w12; b +=  0.32766114*w12;
    auto w13 = c1 * c33; r +=  2.95124691*w13; g +=  2.81201112*w13; b +=  1.17578442*w13;
    auto w14 = c22 * c3; r +=  2.82677043*w14; g +=  0.79933038*w14; b +=  1.81715262*w14;
    auto w15 = c2 * c33; r +=  2.99691099*w15; g +=  1.22593053*w15; b +=  1.80653661*w15;
    auto w16 = c01 * c2; r +=  1.87394106*w16; g +=  2.05027182*w16; b += -0.29835996*w16;
    auto w17 = c01 * c3; r +=  2.56609566*w17; g +=  7.03428198*w17; b +=  0.62575374*w17;
    auto w18 = c02 * c3; r +=  4.08329484*w18; g += -1.40408358*w18; b +=  2.14995522*w18;
    auto w19 = c12 * c3; r +=  6.00078678*w19; g +=  2.55552042*w19; b +=  1.90739502*w19;
    // clang-format on

    return {r, g, b};
}

Latent unmix(Math::Vec3f rgb) {
    auto r01 = clamp01(rgb[0]);
    auto g01 = clamp01(rgb[1]);
    auto b01 = clamp01(rgb[2]);

    auto x = r01 * 63.0;
    auto y = g01 * 63.0;
    auto z = b01 * 63.0;

    isize ix = x;
    isize iy = y;
    isize iz = z;

    auto tx = x - ((f64)ix);
    auto ty = y - ((f64)iy);
    auto tz = z - ((f64)iz);

    auto lut = next(Bytes(_LUT, sizeof(_LUT)), (((ix + iy * 64 + iz * 64 * 64) & 0x3FFFF) * 3));

    auto c0 = 0.0;
    auto c1 = 0.0;
    auto c2 = 0.0;

    // clang-format off
    auto w0 = (1.0 - tx) * (1.0 - ty) * (1.0 - tz); c0 += w0 * lut[  192]; c1 += w0 * lut[  193]; c2 += w0 * lut[  194];
    auto w1 = (      tx) * (1.0 - ty) * (1.0 - tz); c0 += w1 * lut[  195]; c1 += w1 * lut[  196]; c2 += w1 * lut[  197];
    auto w2 = (1.0 - tx) * (      ty) * (1.0 - tz); c0 += w2 * lut[  384]; c1 += w2 * lut[  385]; c2 += w2 * lut[  386];
    auto w3 = (      tx) * (      ty) * (1.0 - tz); c0 += w3 * lut[  387]; c1 += w3 * lut[  388]; c2 += w3 * lut[  389];
    auto w4 = (1.0 - tx) * (1.0 - ty) * (      tz); c0 += w4 * lut[12480]; c1 += w4 * lut[12481]; c2 += w4 * lut[12482];
    auto w5 = (      tx) * (1.0 - ty) * (      tz); c0 += w5 * lut[12483]; c1 += w5 * lut[12484]; c2 += w5 * lut[12485];
    auto w6 = (1.0 - tx) * (      ty) * (      tz); c0 += w6 * lut[12672]; c1 += w6 * lut[12673]; c2 += w6 * lut[12674];
    auto w7 = (      tx) * (      ty) * (      tz); c0 += w7 * lut[12675]; c1 += w7 * lut[12676]; c2 += w7 * lut[12677];
    // clang-format on

    c0 *= 1.0 / 255.0;
    c1 *= 1.0 / 255.0;
    c2 *= 1.0 / 255.0;

    auto c3 = 1.0 - (c0 + c1 + c2);

    auto mixrgb = evalPolynomial(c0, c1, c2, c3);

    return {
        c0,
        c1,
        c2,
        c3,
        r01 - mixrgb[0],
        g01 - mixrgb[1],
        b01 - mixrgb[2]
    };
}

Math::Vec3f mix(Latent latent) {
    auto rgb = evalPolynomial(
        latent[0], latent[1], latent[2], latent[3]
    );

    return {
        clamp01(rgb[0] + latent[4]),
        clamp01(rgb[1] + latent[5]),
        clamp01(rgb[2] + latent[6]),
    };
}

Latent unmixColor(Gfx::Color color) {
    return unmix(srgb2linear(color));
}

Gfx::Color mixColor(Latent latent) {
    return linear2srgb(mix(latent));
};

Math::Vec3f lerp(Math::Vec3f a, Math::Vec3f b, f64 t) {
    auto la = unmix(a);
    auto lb = unmix(b);

    for (usize i = 0; i < LATENT_SIZE; i++) {
        la[i] = Math::lerp(la[i], lb[i], t);
    }

    return mix(la);
}

Gfx::Color lerpColor(Gfx::Color a, Gfx::Color b, f64 t) {
    return linear2srgb(
        lerp(
            srgb2linear(a),
            srgb2linear(b), t
        )
    );
}

} // namespace Mixbox
