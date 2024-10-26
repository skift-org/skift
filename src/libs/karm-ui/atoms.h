#pragma once

#include <karm-gfx/colors.h>

namespace Karm::Ui {

static constexpr isize REM(f64 n) {
    return n * 16;
}

// MARK: Spacing ---------------------------------------------------------------

inline constexpr isize SPACINGPX = 1;
inline constexpr isize SPACING0 = 0;

inline constexpr isize SPACING0_5 = REM(0.125);
inline constexpr isize SPACING1 = REM(0.25);
inline constexpr isize SPACING1_5 = REM(0.375);
inline constexpr isize SPACING2 = REM(0.5);
inline constexpr isize SPACING2_5 = REM(0.625);
inline constexpr isize SPACING3 = REM(0.75);
inline constexpr isize SPACING3_5 = REM(0.875);
inline constexpr isize SPACING4 = REM(1);
inline constexpr isize SPACING5 = REM(1.25);
inline constexpr isize SPACING6 = REM(1.5);
inline constexpr isize SPACING7 = REM(1.75);
inline constexpr isize SPACING8 = REM(2);
inline constexpr isize SPACING9 = REM(2.25);
inline constexpr isize SPACING10 = REM(2.5);
inline constexpr isize SPACING11 = REM(2.75);
inline constexpr isize SPACING12 = REM(3);
inline constexpr isize SPACING14 = REM(3.5);
inline constexpr isize SPACING16 = REM(4);
inline constexpr isize SPACING20 = REM(5);
inline constexpr isize SPACING24 = REM(6);
inline constexpr isize SPACING28 = REM(7);
inline constexpr isize SPACING32 = REM(8);
inline constexpr isize SPACING36 = REM(9);
inline constexpr isize SPACING40 = REM(10);
inline constexpr isize SPACING44 = REM(11);
inline constexpr isize SPACING48 = REM(12);
inline constexpr isize SPACING52 = REM(13);
inline constexpr isize SPACING56 = REM(14);
inline constexpr isize SPACING60 = REM(15);
inline constexpr isize SPACING64 = REM(16);
inline constexpr isize SPACING72 = REM(18);
inline constexpr isize SPACING80 = REM(20);
inline constexpr isize SPACING96 = REM(24);

// MARK: Colors ----------------------------------------------------------------

inline constexpr Gfx::ColorRamp GRAYS = Gfx::ZINC_RAMP;
inline constexpr Gfx::Color GRAY = GRAYS[5];
inline constexpr Gfx::Color GRAY50 = GRAYS[0];
inline constexpr Gfx::Color GRAY100 = GRAYS[1];
inline constexpr Gfx::Color GRAY200 = GRAYS[2];
inline constexpr Gfx::Color GRAY300 = GRAYS[3];
inline constexpr Gfx::Color GRAY400 = GRAYS[4];
inline constexpr Gfx::Color GRAY500 = GRAYS[5];
inline constexpr Gfx::Color GRAY600 = GRAYS[6];
inline constexpr Gfx::Color GRAY700 = GRAYS[7];
inline constexpr Gfx::Color GRAY800 = GRAYS[8];
inline constexpr Gfx::Color GRAY900 = GRAYS[9];
inline constexpr Gfx::Color GRAY950 = GRAYS[10];

inline constexpr Gfx::ColorRamp ACCENTS = Gfx::BLUE_RAMP;
inline constexpr Gfx::Color ACCENT = ACCENTS[5];
inline constexpr Gfx::Color ACCENT50 = ACCENTS[0];
inline constexpr Gfx::Color ACCENT100 = ACCENTS[1];
inline constexpr Gfx::Color ACCENT200 = ACCENTS[2];
inline constexpr Gfx::Color ACCENT300 = ACCENTS[3];
inline constexpr Gfx::Color ACCENT400 = ACCENTS[4];
inline constexpr Gfx::Color ACCENT500 = ACCENTS[5];
inline constexpr Gfx::Color ACCENT600 = ACCENTS[6];
inline constexpr Gfx::Color ACCENT700 = ACCENTS[7];
inline constexpr Gfx::Color ACCENT800 = ACCENTS[8];
inline constexpr Gfx::Color ACCENT900 = ACCENTS[9];
inline constexpr Gfx::Color ACCENT950 = ACCENTS[10];

} // namespace Karm::Ui
