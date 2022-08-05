#pragma once

#include "path.h"
#include "shape.h"
#include "style.h"
#include "surface.h"

namespace Karm::Gfx {

struct Radius {
    double topLeft{};
    double topRight{};
    double bottomLeft{};
    double bottomRight{};

    constexpr Radius() = default;

    constexpr Radius(double all)
        : topLeft(all),
          topRight(all),
          bottomLeft(all),
          bottomRight(all) {}

    constexpr Radius(double topLeft, double topRight, double bottomLeft, double bottomRight)
        : topLeft(topLeft),
          topRight(topRight),
          bottomLeft(bottomLeft),
          bottomRight(bottomRight) {}
};

struct Context {
    struct Scope {
        FillStyle fillStyle{};
        Stroke strokeStyle{};
        Text textStyle{};
        Shadow shadowStyle{};

        Math::Vec2i origin{};
        Math::Recti clip{};
    };

    struct Active {
        double x;
        int sign;
    };

    Surface *_surface{};
    Vec<Scope> _stack{};
    Shape _shape{};
    Path _path{};
    Vec<Active> _active{};
    Vec<double> _scanline;

    /* --- Scope ------------------------------------------------------------ */

    void begin(Surface &c);

    void end();

    Surface &surface();

    Scope &current();

    Scope const &current() const;

    void save();

    void restore();

    /* --- Origin & Clipping ------------------------------------------------ */

    Math::Recti clip() const;

    Math::Vec2i origin() const;

    Math::Recti applyOrigin(Math::Recti rect) const;

    Math::Vec2i applyOrigin(Math::Vec2i pos) const;

    Math::Recti applyClip(Math::Recti rect) const;

    Math::Recti applyAll(Math::Recti rect) const;

    void clip(Math::Recti rect);

    void origin(Math::Vec2i pos);

    /* --- Fill & Stroke ---------------------------------------------------- */

    FillStyle const &fillStyle();

    Stroke const &strokeStyle();

    Text const &textStyle();

    Shadow const &shadowStyle();

    Context &fillStyle(FillStyle style);

    Context &strokeStyle(Stroke style);

    Context &textStyle(Text style);

    Media::Font const &textFont();

    Context &shadowStyle(Shadow style);

    /* --- Drawing ---------------------------------------------------------- */

    void clear(Color color = BLACK);

    void clear(Math::Recti rect, Color color = BLACK);

    /* --- Shapes ----------------------------------------------------------- */

    void plot(Math::Vec2i point);

    void plot(Math::Vec2i point, Color color);

    void stroke(Math::Edgei edge);

    void fill(Math::Edgei edge, double thickness = 1.0f);

    void stroke(Math::Recti rect);

    void fill(Math::Recti rect);

    void stroke(Math::Ellipsei e);

    void fill(Math::Ellipsei e);

    /* --- Text ------------------------------------------------------------- */

    Media::FontMesure mesureRune(Rune rune);

    Media::FontMesure mesureStr(Str text);

    void strokeRune(Math::Vec2i baseline, Rune rune);

    void fillRune(Math::Vec2i baseline, Rune rune);

    void strokeStr(Math::Vec2i baseline, Str str);

    void fillStr(Math::Vec2i baseline, Str str);

    /* --- Paths ------------------------------------------------------------ */

    void _line(Math::Edgei edge, Color color);

    void _trace();

    void _fill(Color color);

    void begin();

    void close();

    void moveTo(Math::Vec2f p, Path::Flags flags = Path::DEFAULT);

    void lineTo(Math::Vec2f p, Path::Flags flags = Path::DEFAULT);

    void hlineTo(double x, Path::Flags flags = Path::DEFAULT);

    void vlineTo(double y, Path::Flags flags = Path::DEFAULT);

    void cubicTo(Math::Vec2f cp1, Math::Vec2f cp2, Math::Vec2f p, Path::Flags flags = Path::DEFAULT);

    void quadTo(Math::Vec2f cp, Math::Vec2f p, Path::Flags flags = Path::DEFAULT);

    void arcTo(Math::Vec2f radius, double angle, Math::Vec2f p, Path::Flags flags = Path::DEFAULT);

    bool evalSvg(Str path);

    void line(Math::Edgef line);

    void rect(Math::Rectf rect);

    void ellipse(Math::Ellipsef ellipse);

    void fill();

    void stroke();

    void shadow();

    /* --- Effects ---------------------------------------------------------- */

    void blur(Math::Recti region, double radius);

    void saturate(Math::Recti region, double saturation);

    void contrast(Math::Recti region, double contrast);

    void brightness(Math::Recti region, double brightness);

    void noise(Math::Recti region, double amount);

    void sepia(Math::Recti region, double amount);

    void tint(Math::Recti region, Color color);
};

} // namespace Karm::Gfx
