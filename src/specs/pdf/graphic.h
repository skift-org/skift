#pragma once

#include <karm-base/enum.h>
#include <karm-io/emit.h>
#include <karm-math/trans.h>

#include "objects.h"

namespace Pdf {

enum struct LineCap {
    BUTT = 0,
    ROUND = 1,
    SQUARE = 2
};

enum struct LineJoin {
    MITER = 0,
    ROUND = 1,
    BEVEL = 2
};

enum struct FillRule {
    NONZERO = 0,
    EVENODD = 1
};

enum struct TextRenderMode {
    FILL = 0,
    STROKE = 1,
    FILL_STROKE = 2,
    INVISIBLE = 3,
    FILL_CLIP = 4,
    STROKE_CLIP = 5,
    FILL_STROKE_CLIP = 6,
    CLIP = 7
};

struct Graphic {
    Io::Emit &e;

    // Graphics state operators

    void save() {
        e("q");
    }

    void restore() {
        e("Q");
    }

    void transform(Math::Trans2f t) {
        e(t.xx);
        e(' ');
        e(t.xy);
        e(' ');
        e(t.yx);
        e(' ');
        e(t.yy);
        e(' ');
        e(t.ox);
        e(' ');
        e(t.oy);
        e(" cm");
    }

    void lineWidth(f64 w) {
        e("{} w", w);
    }

    void lineCap(LineCap cap) {
        e("{} J", toUnderlyingType(cap));
    }

    void lineJoin(LineJoin join) {
        e("{} j", toUnderlyingType(join));
    }

    void miterLimit(f64 m) {
        e("{} M", m);
    }

    void dash(Slice<f64> const &d, f64 o) {
        e("[");
        for (usize i = 0; i < d.len(); ++i) {
            if (i > 0) {
                e(' ');
            }
            e(d[i]);
        }
        e("] ");
        e(o);
        e(" d");
    }

    // Path construction operators

    void moveTo(Math::Vec2f p) {
        e("{} {} m", p.x, p.y);
    }

    void lineTo(Math::Vec2f p) {
        e("{} {} l", p.x, p.y);
    }

    void curveTo(Math::Vec2f c1, Math::Vec2f c2, Math::Vec2f p) {
        e("{} {} {} {} {} {} c", c1.x, c1.y, c2.x, c2.y, p.x, p.y);
    }

    void closePath() {
        e("h");
    }

    void rectangle(Math::Rectf r) {
        e("{} {} {} {} re", r.x, r.y, r.width, r.height);
    }

    // Path painting operators

    void stroke() {
        e("S");
    }

    void closeStroke() {
        e("s");
    }

    void fill(FillRule rule = FillRule::NONZERO) {
        if (rule == FillRule::NONZERO) {
            e("f");
        } else {
            e("f*");
        }
    }

    void fillStroke(FillRule rule = FillRule::NONZERO) {
        if (rule == FillRule::NONZERO) {
            e("B");
        } else {
            e("B*");
        }
    }

    void closeFillStroke(FillRule rule = FillRule::NONZERO) {
        if (rule == FillRule::NONZERO) {
            e("b");
        } else {
            e("b*");
        }
    }

    void endPath() {
        e("n");
    }

    // Clipping path operators

    void clip(FillRule rule = FillRule::NONZERO) {
        if (rule == FillRule::NONZERO) {
            e("W");
        } else {
            e("W*");
        }
    }

    // Text object operators

    void beginText() {
        e("BT");
    }

    void endText() {
        e("ET");
    }

    // Text state operators

    void charSpacing(f64 s) {
        e("{} Tc", s);
    }

    void wordSpacing(f64 s) {
        e("{} Tw", s);
    }

    void horizScaling(f64 s) {
        e("{} Tz", s);
    }

    void textLeading(f64 l) {
        e("{} TL", l);
    }

    void fontSize(Name const &font, f64 size) {
        e("/{} {} Tf", font.value, size);
    }

    void textRenderMode(TextRenderMode mode) {
        e("{} Tr", toUnderlyingType(mode));
    }

    void rise(f64 r) {
        e("{} Ts", r);
    }

    // Text-positioning operators

    void moveText(Math::Vec2f p) {
        e("{} {} Td", p.x, p.y);
    }

    void moveTextSet(Math::Vec2f p) {
        e("{} {} Tm", p.x, p.y);
    }
};

} // namespace Pdf
