module;

#include <karm-gfx/colors.h>

export module Karm.Ui:atoms;

namespace Karm::Ui {

static constexpr isize REM(f64 n) {
    return n * 16;
}

// MARK: Spacing ---------------------------------------------------------------

export constexpr isize SPACINGPX = 1;
export constexpr isize SPACING0 = 0;

export constexpr isize SPACING0_5 = REM(0.125);
export constexpr isize SPACING1 = REM(0.25);
export constexpr isize SPACING1_5 = REM(0.375);
export constexpr isize SPACING2 = REM(0.5);
export constexpr isize SPACING2_5 = REM(0.625);
export constexpr isize SPACING3 = REM(0.75);
export constexpr isize SPACING3_5 = REM(0.875);
export constexpr isize SPACING4 = REM(1);
export constexpr isize SPACING5 = REM(1.25);
export constexpr isize SPACING6 = REM(1.5);
export constexpr isize SPACING7 = REM(1.75);
export constexpr isize SPACING8 = REM(2);
export constexpr isize SPACING9 = REM(2.25);
export constexpr isize SPACING10 = REM(2.5);
export constexpr isize SPACING11 = REM(2.75);
export constexpr isize SPACING12 = REM(3);
export constexpr isize SPACING14 = REM(3.5);
export constexpr isize SPACING16 = REM(4);
export constexpr isize SPACING20 = REM(5);
export constexpr isize SPACING24 = REM(6);
export constexpr isize SPACING28 = REM(7);
export constexpr isize SPACING32 = REM(8);
export constexpr isize SPACING36 = REM(9);
export constexpr isize SPACING40 = REM(10);
export constexpr isize SPACING44 = REM(11);
export constexpr isize SPACING48 = REM(12);
export constexpr isize SPACING52 = REM(13);
export constexpr isize SPACING56 = REM(14);
export constexpr isize SPACING60 = REM(15);
export constexpr isize SPACING64 = REM(16);
export constexpr isize SPACING72 = REM(18);
export constexpr isize SPACING80 = REM(20);
export constexpr isize SPACING96 = REM(24);

// MARK: Colors ----------------------------------------------------------------

export constexpr bool DARK_MODE = true;

export constexpr Gfx::ColorRamp GRAYS = DARK_MODE ? Gfx::ZINC_RAMP : Gfx::ZINC_RAMP.reversed();
export constexpr Gfx::Color GRAY = GRAYS[5];

export constexpr Gfx::Color GRAY50 = GRAYS[0];
export constexpr Gfx::Color GRAY100 = GRAYS[1];
export constexpr Gfx::Color GRAY200 = GRAYS[2];
export constexpr Gfx::Color GRAY300 = GRAYS[3];
export constexpr Gfx::Color GRAY400 = GRAYS[4];
export constexpr Gfx::Color GRAY500 = GRAYS[5];
export constexpr Gfx::Color GRAY600 = GRAYS[6];
export constexpr Gfx::Color GRAY700 = GRAYS[7];
export constexpr Gfx::Color GRAY800 = GRAYS[8];
export constexpr Gfx::Color GRAY900 = GRAYS[9];
export constexpr Gfx::Color GRAY950 = GRAYS[10];

export constexpr Gfx::ColorRamp ACCENTS = DARK_MODE ? Gfx::BLUE_RAMP : Gfx::BLUE_RAMP.reversed();
export constexpr Gfx::Color ACCENT = ACCENTS[5];

export constexpr Gfx::Color ACCENT50 = ACCENTS[0];
export constexpr Gfx::Color ACCENT100 = ACCENTS[1];
export constexpr Gfx::Color ACCENT200 = ACCENTS[2];
export constexpr Gfx::Color ACCENT300 = ACCENTS[3];
export constexpr Gfx::Color ACCENT400 = ACCENTS[4];
export constexpr Gfx::Color ACCENT500 = ACCENTS[5];
export constexpr Gfx::Color ACCENT600 = ACCENTS[6];
export constexpr Gfx::Color ACCENT700 = ACCENTS[7];
export constexpr Gfx::Color ACCENT800 = ACCENTS[8];
export constexpr Gfx::Color ACCENT900 = ACCENTS[9];
export constexpr Gfx::Color ACCENT950 = ACCENTS[10];

} // namespace Karm::Ui
