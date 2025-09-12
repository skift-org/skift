module;

#include <karm-gfx/borders.h>
#include <karm-gfx/color.h>
#include <karm-gfx/outline.h>
#include <karm-math/au.h>
#include <karm-math/vec.h>

export module Vaev.Engine:layout.svg;

import Karm.Core;
import Karm.Scene;

import :style;
import :dom;
import :values;

using namespace Karm;

namespace Vaev::Layout {

// SVG sizes shouldn't be defined using calc values
Opt<PercentOr<Length>> extractValueFromCalc(CalcValue<PercentOr<Length>> const& size) {
    return size.visit(
        Visitor{
            [](CalcValue<PercentOr<Length>>::Value const& v) {
                return Opt<PercentOr<Length>>{v.unwrap<PercentOr<Length>>()};
            },
            [](auto const) {
                return Opt<PercentOr<Length>>{NONE};
            },
        }
    );
}

// FIXME: this should be targeted by the computer refactoring, so we have only resolved percentage or length values
PercentOr<Length> fromSize(Size const& size) {
    if (size.is<Keywords::Auto>())
        return PercentOr<Length>{Percent{100}};

    return extractValueFromCalc(size.unwrap<CalcValue<PercentOr<Length>>>())
        .unwrapOr(PercentOr<Length>{Percent{100}});
}

// FIXME: already present in Resolver obj, but it doesnt make sense to instantiate one here; should also be targeted
// by the computer refactoring
Au resolve(PercentOr<Length> const& value, Au relative) {
    if (auto valueLength = value.is<Length>())
        return Au{valueLength->val()};
    return Au{relative.cast<f64>() * (value.unwrap<Percent>().value() / 100.)};
}

Opt<Gfx::Color> resolve(Paint color, Gfx::Color currentColor) {
    if (color.is<None>())
        return NONE;
    return Vaev::resolve(color.unwrap<Color>(), currentColor);
}

namespace SVG {

Au normalizedDiagonal(Vec2Au relativeTo) {
    return Au{
        Math::sqrt(f64{(relativeTo.x * relativeTo.x) + (relativeTo.y * relativeTo.y)}) / Math::sqrt(2.0)
    };
}

// MARK: Rectangle ----------------------------------------------------------------------------

template <typename T>
struct Rectangle {
    T x;
    T y;
    T width;
    T height;

    template <typename U>
    Rectangle<U> cast() const {
        return {
            static_cast<U>(x),
            static_cast<U>(y),
            static_cast<U>(width),
            static_cast<U>(height),
        };
    }

    Math::Rect<T> toRect() const {
        return {x, y, width, height};
    }

    void repr(Io::Emit& e) const {
        e("(Rectangle {} {} {} {})", x, y, width, height);
    }
};

Rc<Scene::Node> rectToSceneNode(Rectangle<f64> rect, Opt<Gfx::Fill> fill, Opt<Gfx::Fill> strokeColor, f64 strokeWidth) {
    Gfx::Borders borders;
    if (strokeColor and strokeWidth > 0) {
        borders = Gfx::Borders{
            Math::Radiif{},
            Math::Insetsf{strokeWidth},
            Array<Gfx::Fill, 4>::fill(*strokeColor),
            Array<Gfx::BorderStyle, 4>::fill(Gfx::BorderStyle::SOLID),
        };

        // FIXME: needed due to mismatch between SVG's and Scene's box model
        // svg's stroke's center is at the shape's edges
        rect.width += strokeWidth;
        rect.height += strokeWidth;
        rect.x -= strokeWidth / 2;
        rect.y -= strokeWidth / 2;
    }

    return makeRc<Scene::Box>(
        Math::Rectf{rect.x, rect.y, rect.width, rect.height},
        borders,
        Gfx::Outline{},
        fill ? Vec<Gfx::Fill>{fill.unwrap()} : Vec<Gfx::Fill>{}
    );
}

Rectangle<PercentOr<Length>> buildRectangle(Style::SpecifiedValues const& style) {
    return {
        style.svg->x,
        style.svg->y,
        fromSize(style.sizing->width),
        fromSize(style.sizing->height)
    };
}

Rectangle<Au> resolve(Rectangle<PercentOr<Length>> const& rect, Karm::Vec2Au const& relativeTo) {
    return {
        Vaev::Layout::resolve(rect.x, relativeTo.x),
        Vaev::Layout::resolve(rect.y, relativeTo.y),
        Vaev::Layout::resolve(rect.width, relativeTo.x),
        Vaev::Layout::resolve(rect.height, relativeTo.y)
    };
}

// MARK: Circle ----------------------------------------------------------------------------

template <typename T>
struct Circle {
    T cx;
    T cy;
    T r;

    template <typename U>
    Circle<U> cast() const {
        return {
            static_cast<U>(cx),
            static_cast<U>(cy),
            static_cast<U>(r),
        };
    }

    void repr(Io::Emit& e) const {
        e("(Circle {} {} {})", cx, cy, r);
    }
};

Rc<Scene::Node> circleToSceneNode(Circle<f64> circle, Opt<Gfx::Fill> fill, Opt<Gfx::Stroke> stroke) {
    Math::Path path;
    path.ellipse(Math::Ellipse{circle.cx, circle.cy, circle.r});
    return makeRc<Scene::Shape>(path, stroke, fill);
}

Circle<PercentOr<Length>> buildCircle(Style::SpecifiedValues const& style) {
    return {
        style.svg->cx,
        style.svg->cy,
        style.svg->r,
    };
}

Circle<Au> resolve(Circle<PercentOr<Length>> const& circle, Karm::Vec2Au const& relativeTo) {
    return {
        Vaev::Layout::resolve(circle.cx, relativeTo.x),
        Vaev::Layout::resolve(circle.cy, relativeTo.y),
        Vaev::Layout::resolve(circle.r, relativeTo.x),
    };
}

// MARK: Path ----------------------------------------------------------------------------

// https://svgwg.org/svg2-draft/paths.html
Rc<Math::Path> buildPath(Style::SpecifiedValues const& style) {
    if (auto d = style.svg->d.is<String>())
        return Karm::makeRc<Math::Path>(Math::Path::fromSvg(*d));
    return Karm::makeRc<Math::Path>(Math::Path{});
}

// MARK: Shape ----------------------------------------------------------------------------

// https://svgwg.org/svg2-draft/shapes.html#TermShapeElement

struct Shape {
    enum struct Type {
        RECT,
        CIRCLE,
        PATH
    };

    Type type;
    Rc<Style::SpecifiedValues> style;

    static Type _getTypeFromTag(Dom::QualifiedName name) {
        if (name == Svg::PATH_TAG) {
            return Type::PATH;
        } else if (name == Svg::CIRCLE_TAG) {
            return Type::CIRCLE;
        } else if (name == Svg::RECT_TAG) {
            return Type::RECT;
        }
        unreachable();
    }

    static Shape build(Rc<Style::SpecifiedValues> style, Dom::QualifiedName tagName) {
        return {
            _getTypeFromTag(tagName),
            style,
        };
    }

    void repr(Io::Emit& e) const {
        e("(Shape {})", type);
    }
};

template <typename T>
using _Shape = Union<Rectangle<T>, Circle<T>, Rc<Math::Path>>;

struct Frag {
    virtual ~Frag() = default;
    virtual RectAu objectBoundingBox() = 0;
    virtual RectAu strokeBoundingBox() = 0;
    virtual Style::SpecifiedValues const& style() = 0;
};

struct ShapeFrag : Frag {
    _Shape<Au> shape;
    Karm::Cursor<Shape> box;
    Au strokeWidth;

    ShapeFrag(_Shape<Au> shape, Cursor<Shape> box, Au strokeWidth)
        : shape(shape), box(box), strokeWidth(strokeWidth) {}

    static ShapeFrag fromShape(Shape const& shape, Vec2Au relativeTo) {
        Au resolvedStrokeWidth = Vaev::Layout::resolve(shape.style->svg->strokeWidth, normalizedDiagonal(relativeTo));

        switch (shape.type) {
        case Shape::Type::RECT: {
            return {
                resolve(buildRectangle(*shape.style), relativeTo),
                &shape,
                resolvedStrokeWidth,
            };
        }
        case Shape::Type::CIRCLE: {
            return {
                resolve(buildCircle(*shape.style), relativeTo),
                &shape,
                resolvedStrokeWidth,
            };
        }
        case Shape::Type::PATH: {
            return {
                buildPath(*shape.style),
                &shape,
                resolvedStrokeWidth,
            };
        }
        }
        unreachable();
    }

    Rc<Scene::Node> toSceneNode(Gfx::Color currentColor) const {
        Opt<Gfx::Color> resolvedFill = Vaev::Layout::resolve(box->style->svg->fill, currentColor).map([&](auto fill) {
            return fill.withOpacity(box->style->svg->fillOpacity);
        });
        Opt<Gfx::Color> resolvedStrokeColor = Vaev::Layout::resolve(box->style->svg->stroke, currentColor);
        Opt<Gfx::Stroke> resolvedStroke =
            resolvedStrokeColor
                ? Opt<Gfx::Stroke>{{*resolvedStrokeColor, (f64)strokeWidth}}
                : NONE;

        if (auto rect = shape.is<Rectangle<Au>>()) {
            return rectToSceneNode(rect->cast<f64>(), resolvedFill, resolvedStrokeColor, (f64)strokeWidth);
        } else if (auto circle = shape.is<Circle<Au>>()) {
            return circleToSceneNode(circle->cast<f64>(), resolvedFill, resolvedStroke);
        } else if (auto path = shape.is<Rc<Math::Path>>()) {
            return makeRc<Scene::Shape>(*(*path), resolvedStroke, resolvedFill);
        };
        unreachable();
    }

    RectAu objectBoundingBox() override {
        if (auto rect = shape.is<Rectangle<Au>>()) {
            return rect->toRect();
        } else if (auto circle = shape.is<Circle<Au>>()) {
            Math::Path path;
            path.ellipse(Math::Ellipse{circle->cx, circle->cy, circle->r}.cast<f64>());
            return path.bound().cast<Au>();
        } else if (auto path = shape.is<Rc<Math::Path>>()) {
            return (*path)->bound().cast<Au>();
        };
        unreachable();
    }

    RectAu strokeBoundingBox() override {
        return objectBoundingBox().grow((Au)(strokeWidth / 2_au));
    }

    Style::SpecifiedValues const& style() override {
        return *box->style;
    }

    void repr(Io::Emit& e) const {
        e("(ShapeFrag {} {})", shape, strokeWidth);
    }
};

bool isShape(Dom::QualifiedName name) {
    return name == Svg::PATH_TAG or name == Svg::CIRCLE_TAG or name == Svg::RECT_TAG;
}

// MARK: Root ----------------------------------------------------------------------------

// https://svgwg.org/svg2-draft/coords.html#ComputingAViewportsTransform
Math::Trans2f computeEquivalentTransformOfSVGViewport(ViewBox const& vb, Vec2Au const& position, Vec2Au const& size) {
    // 1. Let vb-x, vb-y, vb-width, vb-height be the min-x, min-y, width and height values of the viewBox attribute
    // respectively.
    // 2. Let e-x, e-y, e-width, e-height be the position and size of the element respectively.

    // 3. Let align be the align value of preserveAspectRatio, or 'xMidYMid' if preserveAspectRatio is not defined.
    // FIXME: preserveAspectRatio still not implemented
    Opt<AlignAxisSVG> align{AlignAxisSVG{AlignAxisSVG::MID, AlignAxisSVG::MID}};

    // 4. Let meetOrSlice be the meetOrSlice value of preserveAspectRatio, or 'meet' if preserveAspectRatio is not
    // defined or if meetOrSlice is missing from this value.
    // FIXME: preserveAspectRatio still not implemented
    MeetOrSlice meetOrSlice{MeetOrSlice::MEET};

    // 5. Initialize scale-x to e-width/vb-width.
    f64 scaleX = (f64)size.x / vb.width;

    // 6. Initialize scale-y to e-height/vb-height.
    f64 scaleY = (f64)size.y / vb.height;

    // 7. If align is not 'none' and meetOrSlice is 'meet', set the larger of scale-x and scale-y to the smaller.
    // 8. Otherwise, if align is not 'none' and meetOrSlice is 'slice', set the smaller of scale-x and scale-y to
    // the larger.
    if (align != NONE and meetOrSlice == MeetOrSlice::MEET) {
        scaleY = scaleX = min(scaleX, scaleY);
    } else if (align != NONE and meetOrSlice == MeetOrSlice::SLICE) {
        scaleY = scaleX = max(scaleX, scaleY);
    }

    // 9. Initialize translate-x to e-x - (vb-x * scale-x).
    f64 translateX = (f64)position.x - (vb.minX * scaleX);

    // 10. Initialize translate-y to e-y - (vb-y * scale-y)
    f64 translateY = (f64)position.y - (vb.minY * scaleY);

    if (align) {
        // 11. If align contains 'xMid', add (e-width - vb-width * scale-x) / 2 to translate-x.
        if (align->x == AlignAxisSVG::MID) {
            translateX += ((f64)size.x - vb.width * scaleX) / 2;
        }

        // 12. If align contains 'xMax', add (e-width - vb-width * scale-x) to translate-x.
        if (align->x == AlignAxisSVG::MAX) {
            translateX += ((f64)size.x - vb.width * scaleX);
        }

        // 13. If align contains 'yMid', add (e-height - vb-height * scale-y) / 2 to translate-y.
        if (align->y == AlignAxisSVG::MID) {
            translateY += ((f64)size.y - vb.height * scaleY) / 2;
        }

        // 14. If align contains 'yMax', add (e-height - vb-height * scale-y) to translate-y.
        if (align->y == AlignAxisSVG::MAX) {
            translateY += ((f64)size.y - vb.height * scaleY);
        }
    }

    return Math::Trans2f::translate({(f64)translateX, (f64)translateY}).scaled({(f64)scaleX, (f64)scaleY});
}

// https://svgwg.org/svg2-draft/coords.html#SizingSVGInCSS
Opt<Number> intrinsicAspectRatio(Opt<ViewBox> const& vb, Size const& width, Size const& height) {
    // FIXME: again this should be targetted by the styling computation refactoring,
    // where Size will be resolved to a mix between Percent and Lengths
    auto absoluteValue = [](Size size) -> Opt<Length> {
        if (not size.is<CalcValue<PercentOr<Length>>>())
            return NONE;

        auto calc = size.unwrap<CalcValue<PercentOr<Length>>>();

        auto percOrLength = extractValueFromCalc(calc);
        if (not percOrLength)
            return NONE;

        if (percOrLength->is<Percent>())
            return NONE;

        return percOrLength->unwrap<Length>();
    };

    auto absWidth = absoluteValue(width);
    auto absHeight = absoluteValue(height);

    // 1. If the width and height sizing properties on the ‘svg’ element are both absolute values:
    if (absWidth and absHeight)
        return absWidth->val() / absHeight->val();

    // 2. If an SVG View is active:
    // TODO

    // 3. If the ‘viewBox’ on the ‘svg’ element is correctly specified:
    if (vb) {
        return (f64)vb->width / vb->height;
    }

    return NONE;
}

} // namespace SVG

} // namespace Vaev::Layout
