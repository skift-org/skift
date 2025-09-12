#pragma once

import Karm.Core;

#include <karm-gfx/canvas.h>
#include <karm-logger/logger.h>

namespace Karm::Font::Ttf {

static constexpr bool DEBUG_GLYF = false;

struct Glyf : Io::BChunk {
    static constexpr Str SIG = "glyf";

    static constexpr u8 ON_CURVE_POINT = 0x01;
    static constexpr u8 X_SHORT_VECTOR = 0x02;
    static constexpr u8 Y_SHORT_VECTOR = 0x04;
    static constexpr u8 REPEAT = 0x08;
    static constexpr u8 SAME_OR_POSITIVE_X = 0x10;
    static constexpr u8 SAME_OR_POSITIVE_Y = 0x20;
    static constexpr u8 OVERLAY_SIMPLE = 0x40;

    struct Metrics {
        i16 numContours;
        i16 xMin;
        i16 yMin;
        i16 xMax;
        i16 yMax;
    };

    always_inline Metrics metrics(Io::BScan& s, usize glyfOffset) const {
        s.skip(glyfOffset);
        auto numContours = s.nextI16be();
        if (numContours == 0) {
            return {};
        }
        auto xMin = s.nextI16be();
        auto yMin = s.nextI16be();
        auto xMax = s.nextI16be();
        auto yMax = s.nextI16be();
        return {numContours, xMin, yMin, xMax, yMax};
    }

    always_inline Metrics metrics(usize glyfOffset) const {
        auto s = begin();
        return metrics(s, glyfOffset);
    }

    struct SimpleContour {
        enum Flag {
            ON_CURVE_POINT = 0x01,
            X_SHORT_VECTOR = 0x02,
            Y_SHORT_VECTOR = 0x04,
            REPEAT = 0x08,
            X_SAME_OR_POSITIVE_X_SHORT_VECTOR = 0x10,
            Y_SAME_OR_POSITIVE_Y_SHORT_VECTOR = 0x20,
            OVERLAP_SIMPLE = 0x40,
        };

        u8 flags;
        i16 x;
        i16 y;
    };

    void contourSimple(Gfx::Canvas& g, Metrics m, Io::BScan& s) const {
        auto endPtsOfContours = s;
        auto nPoints = s.peek(2 * (m.numContours - 1)).nextU16be() + 1u;
        u16 instructionLength = s.skip(m.numContours * 2).nextU16be();

        auto flagsScan = s.skip(instructionLength);

        usize nXCoords = 0;
        u8 flags = 0;
        u8 flagsRepeat = 0;

        for (usize i = 0; i < nPoints; i++) {
            if (not flagsRepeat) {
                flags = s.nextU8be();
                if (flags & REPEAT) {
                    flagsRepeat = s.nextU8be();
                }
            } else {
                flagsRepeat--;
            }

            nXCoords += flags & SimpleContour::X_SHORT_VECTOR
                            ? 1
                            : (flags & SAME_OR_POSITIVE_X ? 0 : 2);
        }

        auto xCoordsScan = s;
        auto yCoordsScan = s.skip(nXCoords);

        usize start = 0;
        Math::Vec2f curr{};
        flags = 0;
        flagsRepeat = 0;
        for (isize c = 0; c < m.numContours; c++) {
            usize end = endPtsOfContours.nextU16be();

            Math::Vec2f cp{};
            Math::Vec2f startP{};
            bool wasCp = false;

            for (usize i = start; i <= end; i++) {
                if (not flagsRepeat) {
                    flags = flagsScan.nextU8be();
                    if (flags & REPEAT) {
                        flagsRepeat = flagsScan.nextU8be();
                    }
                } else {
                    flagsRepeat--;
                }

                isize x = (flags & X_SHORT_VECTOR)
                              ? ((flags & SAME_OR_POSITIVE_X) ? xCoordsScan.nextU8be() : -xCoordsScan.nextU8be())
                              : ((flags & SAME_OR_POSITIVE_X) ? 0 : xCoordsScan.nextI16be());

                isize y = (flags & Y_SHORT_VECTOR)
                              ? ((flags & SAME_OR_POSITIVE_Y) ? yCoordsScan.nextU8be() : -yCoordsScan.nextU8be())
                              : ((flags & SAME_OR_POSITIVE_Y) ? 0 : yCoordsScan.nextI16be());

                curr = curr + Math::Vec2f{(f64)x, (f64)-y};

                if (i == start) {
                    g.moveTo(curr);
                    startP = curr;
                } else {
                    if (flags & ON_CURVE_POINT) {
                        if (wasCp) {
                            g.quadTo(cp, curr);
                        } else {
                            g.lineTo(curr);
                        }
                        wasCp = false;
                    } else {
                        if (wasCp) {
                            auto p1 = (cp + curr) / 2;
                            g.quadTo(cp, p1);
                            cp = curr;
                            wasCp = true;
                        } else {
                            cp = curr;
                            wasCp = true;
                        }
                    }
                }
            }

            if (wasCp) {
                g.quadTo(cp, startP);
            }

            g.closePath();
            start = end + 1;
        }
    }

    void contourComposite(Gfx::Canvas&, Metrics, Io::BScan&) const {
        logDebugIf(DEBUG_GLYF, "composite glyph not implemented");
    }

    void contour(Gfx::Canvas& g, usize glyfOffset) const {
        auto s = begin();
        auto m = metrics(s, glyfOffset);

        if (m.numContours > 0) {
            contourSimple(g, m, s);
        } else if (m.numContours < 0) {
            contourComposite(g, m, s);
        }
    }
};

} // namespace Karm::Font::Ttf
