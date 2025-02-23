#include <karm-gfx/color.h>
#include <karm-gfx/colors.h>
#include <karm-test/macros.h>

namespace Karm::Gfx::Tests {

test$("karm-test-hsl-rgb-conversion-basic") {
    expectEq$(Gfx::hslToRgb(Gfx::Hsl{0, 0, 0}), Gfx::BLACK);
    expectEq$(Gfx::hslToRgb(Gfx::Hsl{0, 0, 1}), Gfx::WHITE);
    expectEq$(Gfx::hslToRgb(Gfx::Hsl{0, 1, .5}), Gfx::Color::fromRgb(255, 0, 0));
    expectEq$(Gfx::hslToRgb(Gfx::Hsl{120, 1, .5}), Gfx::Color::fromRgb(0, 255, 0));
    expectEq$(Gfx::hslToRgb(Gfx::Hsl{240, 1, .5}), Gfx::Color::fromRgb(0, 0, 255));

    expectEq$(Gfx::rgbToHsl(Gfx::BLACK), (Gfx::Hsl{0, 0.0, 0.0}));
    expectEq$(Gfx::rgbToHsl(Gfx::WHITE), (Gfx::Hsl{0, 0.0, 1.0}));
    expectEq$(Gfx::rgbToHsl(Gfx::Color::fromRgb(255, 0, 0)), (Gfx::Hsl{0, 1.0, .5}));
    expectEq$(Gfx::rgbToHsl(Gfx::Color::fromRgb(0, 255, 0)), (Gfx::Hsl{120, 1.0, .5}));
    expectEq$(Gfx::rgbToHsl(Gfx::Color::fromRgb(0, 0, 255)), (Gfx::Hsl{240, 1.0, .5}));

    return Ok();
}

test$("karm-test-hsl-rgb-conversion-arbitrary") {
    expectEq$(Gfx::hslToRgb(Gfx::rgbToHsl(Gfx::Color::fromRgb(1, 123, 32))), Gfx::Color::fromRgb(1, 123, 32));
    expectEq$(Gfx::hslToRgb(Gfx::rgbToHsl(Gfx::Color::fromRgb(119, 172, 235))), Gfx::Color::fromRgb(119, 172, 235));
    expectEq$(Gfx::hslToRgb(Gfx::rgbToHsl(Gfx::Color::fromRgb(31, 253, 29))), Gfx::Color::fromRgb(31, 253, 29));
    expectEq$(Gfx::hslToRgb(Gfx::rgbToHsl(Gfx::Color::fromRgb(84, 219, 4))), Gfx::Color::fromRgb(84, 219, 4));
    expectEq$(Gfx::hslToRgb(Gfx::rgbToHsl(Gfx::Color::fromRgb(19, 67, 199))), Gfx::Color::fromRgb(19, 67, 199));

    return Ok();
}

} // namespace Karm::Gfx::Tests
