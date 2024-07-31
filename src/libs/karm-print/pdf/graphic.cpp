#include "graphic.h"

namespace Pdf {

// Graphics state operators

void Graphic::save() {
    e("q");
}
void Graphic::restore() {
    e("Q");
}
void Graphic::transform(Math::Trans2f t) {
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
void Graphic::lineWidth(f64 w) {
    e("{} w", w);
}
void Graphic::lineCap(LineCap cap) {
    e("{} J", toUnderlyingType(cap));
}
void Graphic::lineJoin(LineJoin join) {
    e("{} j", toUnderlyingType(join));
}
void Graphic::miterLimit(f64 m) {
    e("{} M", m);
}
void Graphic::dash(Slice<f64> const &d, f64 o) {
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

void Graphic::moveTo(Math::Vec2f p) {
    e("{} {} m", p.x, p.y);
}

void Graphic::lineTo(Math::Vec2f p) {
    e("{} {} l", p.x, p.y);
}

void Graphic::curveTo(Math::Vec2f c1, Math::Vec2f c2, Math::Vec2f p) {
    e("{} {} {} {} {} {} c", c1.x, c1.y, c2.x, c2.y, p.x, p.y);
}

void Graphic::closePath() {
    e("h");
}

void Graphic::rectangle(Math::Rectf r) {
    e("{} {} {} {} re", r.x, r.y, r.width, r.height);
}

// Path painting operators

void Graphic::stroke() {
    e("S");
}

void Graphic::closeStroke() {
    e("s");
}

void Graphic::fill(FillRule rule) {
    if (rule == FillRule::NONZERO) {
        e("f");
    } else {
        e("f*");
    }
}

void Graphic::fillStroke(FillRule rule) {
    if (rule == FillRule::NONZERO) {
        e("B");
    } else {
        e("B*");
    }
}

void Graphic::closeFillStroke(FillRule rule) {
    if (rule == FillRule::NONZERO) {
        e("b");
    } else {
        e("b*");
    }
}

void Graphic::endPath() {
    e("n");
}

// Clipping path operators

void Graphic::clip(FillRule rule) {
    if (rule == FillRule::NONZERO) {
        e("W");
    } else {
        e("W*");
    }
}

// Text object operators

void Graphic::beginText() {
    e("BT");
}

void Graphic::endText() {
    e("ET");
}

// Text state operators

void Graphic::charSpacing(f64 s) {
    e("{} Tc", s);
}

void Graphic::wordSpacing(f64 s) {
    e("{} Tw", s);
}

void Graphic::horizScaling(f64 s) {
    e("{} Tz", s);
}

void Graphic::textLeading(f64 l) {
    e("{} TL", l);
}

void Graphic::fontSize(Name const &font, f64 size) {
    e("/{} {} Tf", font.str(), size);
}

void Graphic::textRenderMode(TextRenderMode mode) {
    e("{} Tr", toUnderlyingType(mode));
}

void Graphic::rise(f64 r) {
    e("{} Ts", r);
}

void Graphic::moveText(Math::Vec2f p) {
    e("{} {} Td", p.x, p.y);
}

void Graphic::moveTextSet(Math::Vec2f p) {
    e("{} {} Tm", p.x, p.y);
}

// Text-positioning operators

} // namespace Pdf
