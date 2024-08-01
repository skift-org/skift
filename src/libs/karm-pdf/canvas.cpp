#include "canvas.h"

namespace Karm::Pdf {

void Canvas::begin() {
    emit().ln("q");
}

void Canvas::close() {
    emit().ln("Q");
}

void Canvas::moveTo(Math::Vec2f p, Gfx::Path::Flags flags) {
    p = _mapPointAndUpdate(p, flags);
    emit().ln("{} {} m", p.x, p.y);
}

void Canvas::lineTo(Math::Vec2f p, Gfx::Path::Flags flags) {
    p = _mapPointAndUpdate(p, flags);
    emit().ln("{} {} l", p.x, p.y);
}

void Canvas::hlineTo(f64 x, Gfx::Path::Flags flags) {
    auto p = _mapPoint({x, 0}, flags);
    emit().ln("{} 0 l", p.x);
}

void Canvas::vlineTo(f64 y, Gfx::Path::Flags flags) {
    auto p = _mapPoint({0, y}, flags);
    emit().ln("0 {} l", p.y);
}

void Canvas::cubicTo(Math::Vec2f cp1, Math::Vec2f cp2, Math::Vec2f p, Gfx::Path::Flags flags) {
    cp1 = _mapPoint(cp1, flags);
    cp2 = _mapPoint(cp2, flags);
    p = _mapPointAndUpdate(p, flags);
    emit().ln("{} {} {} {} {} {} c", cp1.x, cp1.y, cp2.x, cp2.y, p.x, p.y);
}

void Canvas::quadTo(Math::Vec2f cp, Math::Vec2f p, Gfx::Path::Flags flags) {
    cp = _mapPoint(cp, flags);
    p = _mapPointAndUpdate(p, flags);
    emit().ln("{} {} {} {} q", cp.x, cp.y, p.x, p.y);
}

void Canvas::arcTo(Math::Vec2f, f64, Math::Vec2f, Gfx::Path::Flags) {
    notImplemented();
}

void Canvas::line(Math::Edgef line) {
    moveTo(line.start, Gfx::Path::DEFAULT);
    lineTo(line.end, Gfx::Path::DEFAULT);
}

void Canvas::curve(Math::Curvef curve) {
    moveTo(curve.a, Gfx::Path::DEFAULT);
    cubicTo(curve.b, curve.c, curve.d, Gfx::Path::DEFAULT);
}

void Canvas::ellipse(Math::Ellipsef) {
    notImplemented();
}

void Canvas::rect(Math::Rectf rect, Math::Radiif) {
    moveTo(rect.topStart(), Gfx::Path::DEFAULT);
    lineTo(rect.topEnd(), Gfx::Path::DEFAULT);
    lineTo(rect.bottomEnd(), Gfx::Path::DEFAULT);
    lineTo(rect.bottomStart(), Gfx::Path::DEFAULT);
    close();
}

void Canvas::fill(Gfx::Fill fill, Gfx::FillRule rule) {
    auto color = fill.unwrap<Gfx::Color>();
    emit().ln("{} {} {} rg", color.red, color.green, color.blue);
    if (rule == Gfx::FillRule::NONZERO)
        emit().ln("f");
    else
        emit().ln("f*");
}

void Canvas::stroke(Gfx::Stroke style) {
    auto color = style.fill.unwrap<Gfx::Color>();
    emit().ln("{} {} {} RG", color.red, color.green, color.blue);

    emit().ln("{} w", style.width);

    if (style.cap == Gfx::ROUND_CAP)
        emit().ln("1 J");
    else if (style.cap == Gfx::SQUARE_CAP)
        emit().ln("2 J");
    else
        emit().ln("0 J");

    if (style.join == Gfx::ROUND_JOIN)
        emit().ln("1 j");
    else if (style.join == Gfx::BEVEL_JOIN)
        emit().ln("2 j");
    else
        emit().ln("0 j");

    emit().ln("S");
}

} // namespace Karm::Pdf
