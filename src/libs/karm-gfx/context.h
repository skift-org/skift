#pragma once

#include <karm-media/icon.h>

#include "buffer.h"
#include "path.h"
#include "shape.h"
#include "style.h"

namespace Karm::Gfx {

struct Context {
    struct Scope {
        FillStyle fillStyle{};
        StrokeStyle strokeStyle{};
        Media::Font textFont = Media::Font::fallback();
        ShadowStyle shadowStyle{};

        Math::Vec2i origin{};
        Math::Recti clip{};
        Math::Trans2f trans = Math::Trans2f::identity();

        Math::Trans2f transWithOrigin() {
            return trans.translated(origin.x, origin.y);
        }
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

    /* --- Transform -------------------------------------------------------- */

    void transform(Math::Trans2f trans);

    void translate(Math::Vec2f pos);

    void scale(Math::Vec2f pos);

    void rotate(double angle);

    void skew(Math::Vec2f pos);

    void identity();

    /* --- Fill & Stroke ---------------------------------------------------- */

    FillStyle const &fillStyle();

    StrokeStyle const &strokeStyle();

    Media::Font const &textFont();

    ShadowStyle const &shadowStyle();

    Context &fillStyle(FillStyle style);

    Context &strokeStyle(StrokeStyle style);

    Context &textFont(Media::Font style);

    Context &shadowStyle(ShadowStyle style);

    /* --- Drawing ---------------------------------------------------------- */

    void clear(Color color = BLACK);

    void clear(Math::Recti rect, Color color = BLACK);

    /* --- Blitting --------------------------------------------------------- */

    void blit(Math::Recti src, Math::Recti dest, Surface surface);

    void blit(Math::Recti dest, Surface surface);

    void blit(Math::Vec2i dest, Surface surface);

    /* --- Shapes ----------------------------------------------------------- */

    void plot(Math::Vec2i point);

    void plot(Math::Vec2i point, Color color);

    void stroke(Math::Edgei edge);

    void fill(Math::Edgei edge, double thickness = 1.0f);

    void stroke(Math::Recti rect, BorderRadius radius = 0);

    void fill(Math::Recti rect, BorderRadius radius = 0);

    void stroke(Math::Ellipsei e);

    void fill(Math::Ellipsei e);

    void stroke(Math::Vec2i pos, Media::Icon icon);

    void fill(Math::Vec2i pos, Media::Icon icon);

    void stroke(Math::Vec2i baseline, Rune rune);

    void fill(Math::Vec2i baseline, Rune rune);

    void stroke(Math::Vec2i baseline, Str str);

    void fill(Math::Vec2i baseline, Str str);

    /* --- Debug ------------------------------------------------------------ */

    void _line(Math::Edgei edge, Color color);

    void _rect(Math::Recti rect, Color color);

    void _arrow(Math::Vec2i from, Math::Vec2i to, Color color);

    void _doubleArrow(Math::Vec2i from, Math::Vec2i to, Color color);

    void _trace();

    /* --- Paths ------------------------------------------------------------ */

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

    void rect(Math::Rectf rect, BorderRadius radius = 0);

    void ellipse(Math::Ellipsef ellipse);

    void fill();

    void fill(FillStyle style);

    void stroke();

    void stroke(StrokeStyle style);

    void shadow();

    void shadow(ShadowStyle style);

    /* --- Effects ---------------------------------------------------------- */

    void blur(Math::Recti region, int radius);

    void saturate(Math::Recti region, double saturation);

    void grayscale(Math::Recti region);

    void contrast(Math::Recti region, double contrast);

    void brightness(Math::Recti region, double brightness);

    void noise(Math::Recti region, double amount);

    void sepia(Math::Recti region, double amount);

    void tint(Math::Recti region, Color color);
};

} // namespace Karm::Gfx
