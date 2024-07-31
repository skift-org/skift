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

    // MARK: Graphics state operators ------------------------------------------

    void save();

    void restore();

    void transform(Math::Trans2f t);

    void lineWidth(f64 w);

    void lineCap(LineCap cap);

    void lineJoin(LineJoin join);

    void miterLimit(f64 m);

    void dash(Slice<f64> const &d, f64 o);

    // MARK: Path construction operators ---------------------------------------

    void moveTo(Math::Vec2f p);

    void lineTo(Math::Vec2f p);

    void curveTo(Math::Vec2f c1, Math::Vec2f c2, Math::Vec2f p);

    void closePath();

    void rectangle(Math::Rectf r);

    // MARK: Path painting operators -------------------------------------------

    void stroke();

    void closeStroke();

    void fill(FillRule rule = FillRule::NONZERO);

    void fillStroke(FillRule rule = FillRule::NONZERO);

    void closeFillStroke(FillRule rule = FillRule::NONZERO);

    void endPath();

    // MARK: Clipping path operators -------------------------------------------

    void clip(FillRule rule = FillRule::NONZERO);

    // MARK: Text object operators ---------------------------------------------

    void beginText();

    void endText();

    // MARK: Text state operators ----------------------------------------------

    void charSpacing(f64 s);

    void wordSpacing(f64 s);

    void horizScaling(f64 s);

    void textLeading(f64 l);

    void fontSize(Name const &font, f64 size);

    void textRenderMode(TextRenderMode mode);

    void rise(f64 r);

    // MARK: Text-positioning operators ----------------------------------------

    void moveText(Math::Vec2f p);

    void moveTextSet(Math::Vec2f p);
};

} // namespace Pdf
