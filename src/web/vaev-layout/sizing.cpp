#include "sizing.h"

#include "values.h"

// https://www.w3.org/TR/css-box-4
// https://www.w3.org/TR/css-sizing-3

namespace Vaev::Layout {

// MARK: Insets ----------------------------------------------------------------

Px computeInset(Tree &t, Frag &f, Axis, Length inset) {
    return resolve(t, f, inset);
}

Px computeInset(Tree &t, Frag &f, Input input, Axis axis, PercentOr<Length> inset) {
    return resolve(t, f, inset, input.containingBlock.wh[axis.index()]);
}

Px computeInset(Tree &t, Frag &f, Input input, Axis axis, Width inset) {
    return resolve(t, f, inset, input.containingBlock.wh[axis.index()]);
}

Px allInsets(Tree &t, Frag &f, Input input, Axis axis) {
    Px res = Px{0};
    if (axis == Axis::HORIZONTAL) {
        res += computeInset(t, f, input, axis, f->padding->start);
        res += computeInset(t, f, input, axis, f->padding->end);

        if (f->borders->start.style != BorderStyle::NONE)
            res += computeInset(t, f, axis, f->borders->start.width);

        if (f->borders->end.style != BorderStyle::NONE)
            res += computeInset(t, f, axis, f->borders->end.width);

        res += computeInset(t, f, input, axis, f->margin->start);
        res += computeInset(t, f, input, axis, f->margin->end);
    } else {
        res += computeInset(t, f, input, axis, f->padding->top);
        res += computeInset(t, f, input, axis, f->padding->bottom);

        if (f->borders->top.style != BorderStyle::NONE)
            res += computeInset(t, f, axis, f->borders->top.width);

        if (f->borders->bottom.style != BorderStyle::NONE)
            res += computeInset(t, f, axis, f->borders->bottom.width);

        res += computeInset(t, f, input, axis, f->margin->top);
        res += computeInset(t, f, input, axis, f->margin->bottom);
    }

    return res;
}

InsetsPx computePadding(Tree &t, Frag &f, Input input) {
    InsetsPx res;

    res.top = computeInset(t, f, input, Axis::VERTICAL, f->padding->top);
    res.end = computeInset(t, f, input, Axis::HORIZONTAL, f->padding->end);
    res.bottom = computeInset(t, f, input, Axis::VERTICAL, f->padding->bottom);
    res.start = computeInset(t, f, input, Axis::HORIZONTAL, f->padding->start);

    return res;
}

InsetsPx computeBorders(Tree &t, Frag &f) {
    InsetsPx res;

    if (f->borders->top.style != BorderStyle::NONE)
        res.top = computeInset(t, f, Axis::VERTICAL, f->borders->top.width);

    if (f->borders->end.style != BorderStyle::NONE)
        res.end = computeInset(t, f, Axis::HORIZONTAL, f->borders->end.width);

    if (f->borders->bottom.style != BorderStyle::NONE)
        res.bottom = computeInset(t, f, Axis::VERTICAL, f->borders->bottom.width);

    if (f->borders->start.style != BorderStyle::NONE)
        res.start = computeInset(t, f, Axis::HORIZONTAL, f->borders->start.width);

    return res;
}

RadiiPx computeRadii(Tree &t, Frag &f, Input input) {
    RadiiPx res;

    res.a = computeInset(t, f, input, Axis::VERTICAL, f->borders->radii.a);
    res.b = computeInset(t, f, input, Axis::HORIZONTAL, f->borders->radii.b);
    res.c = computeInset(t, f, input, Axis::HORIZONTAL, f->borders->radii.c);
    res.d = computeInset(t, f, input, Axis::VERTICAL, f->borders->radii.d);
    res.e = computeInset(t, f, input, Axis::VERTICAL, f->borders->radii.e);
    res.f = computeInset(t, f, input, Axis::HORIZONTAL, f->borders->radii.f);
    res.g = computeInset(t, f, input, Axis::HORIZONTAL, f->borders->radii.g);
    res.h = computeInset(t, f, input, Axis::VERTICAL, f->borders->radii.h);

    return res;
}

InsetsPx computeMargins(Tree &t, Frag &f, Input input) {
    InsetsPx res;

    res.top = computeInset(t, f, input, Axis::VERTICAL, f->margin->top);
    res.end = computeInset(t, f, input, Axis::HORIZONTAL, f->margin->end);
    res.bottom = computeInset(t, f, input, Axis::VERTICAL, f->margin->bottom);
    res.start = computeInset(t, f, input, Axis::HORIZONTAL, f->margin->start);

    return res;
}

// MARK: Content ---------------------------------------------------------------

Px computeSpecifiedSize(Tree &t, Frag &f, Input input, Axis axis, Size size, Px availableSpace) {
    Px res = Px{0};

    availableSpace = max(Px{0}, availableSpace - allInsets(t, f, input, axis));

    if (size == Size::MIN_CONTENT) {
        res = measure(
            t, f,
            axis,
            IntrinsicSize::MIN_CONTENT,
            availableSpace
        );
    } else if (size == Size::MAX_CONTENT) {
        res = measure(
            t, f,
            axis,
            IntrinsicSize::MAX_CONTENT,
            Px{0}
        );
    } else if (size == Size::AUTO) {
        res = measure(
            t, f,
            axis,
            IntrinsicSize::MAX_CONTENT,
            availableSpace
        );
    } else if (size == Size::FIT_CONTENT) {
        res = measure(
            t, f,
            axis,
            IntrinsicSize::MAX_CONTENT,
            resolve(t, f, size.value, input.containingBlock.wh[axis.index()])
        );
    } else if (size == Size::LENGTH) {
        res = resolve(t, f, size.value, input.containingBlock.width);
    } else {
        logWarn("unknown specified size: {}", size);
        res = Px{0};
    }

    if (f->boxSizing == BoxSizing::CONTENT_BOX)
        return res;

    if (axis == Axis::HORIZONTAL) {
        res += max(Px{0}, computeInset(t, f, input, axis, f->padding->start));
        res += max(Px{0}, computeInset(t, f, input, axis, f->padding->end));

        if (f->borders->start.style != BorderStyle::NONE)
            res += max(Px{0}, computeInset(t, f, axis, f->borders->start.width));

        if (f->borders->end.style != BorderStyle::NONE)
            res += max(Px{0}, computeInset(t, f, axis, f->borders->end.width));
    } else {
        res += max(Px{0}, computeInset(t, f, input, axis, f->padding->top));
        res += max(Px{0}, computeInset(t, f, input, axis, f->padding->bottom));

        if (f->borders->top.style != BorderStyle::NONE)
            res += max(Px{0}, computeInset(t, f, axis, f->borders->top.width));

        if (f->borders->bottom.style != BorderStyle::NONE)
            res += max(Px{0}, computeInset(t, f, axis, f->borders->bottom.width));
    }

    return res;
}

// https://www.w3.org/TR/css-sizing-3/#preferred-size-properties
Px computePreferredSize(Tree &t, Frag &f, Input input, Axis axis, Px availableSpace) {
    auto specifiedSize = f->sizing->size(axis);

    Px res = computeSpecifiedSize(t, f, input, axis, specifiedSize, availableSpace);

    auto minSize = f->sizing->minSize(axis);
    if (minSize != Size::AUTO) {
        res = max(res, computeSpecifiedSize(t, f, input, axis, minSize, availableSpace));
    }

    auto maxSize = f->sizing->maxSize(axis);
    if (maxSize != Size::NONE) {
        res = min(res, computeSpecifiedSize(t, f, input, axis, maxSize, availableSpace));
    }

    return res;
}

Px computePreferredContentSize(Tree &t, Frag &f, Input input, Axis axis, Px availableSpace) {
    auto specifiedSize = f->sizing->size(axis);

    return computeSpecifiedSize(t, f, input, axis, specifiedSize, availableSpace);
}

// https://www.w3.org/TR/css-box-4/#border-box
Px computePreferredBorderSize(Tree &t, Frag &f, Input input, Axis axis, Px availableSpace) {
    auto res = computePreferredSize(t, f, input, axis, availableSpace);

    if (f->boxSizing == BoxSizing::BORDER_BOX)
        return res;

    if (axis == Axis::HORIZONTAL) {
        res += computeInset(t, f, input, axis, f->padding->start);
        res += computeInset(t, f, input, axis, f->padding->end);

        if (f->borders->start.style != BorderStyle::NONE)
            res += computeInset(t, f, axis, f->borders->start.width);

        if (f->borders->end.style != BorderStyle::NONE)
            res += computeInset(t, f, axis, f->borders->end.width);
    } else {
        res += computeInset(t, f, input, axis, f->padding->top);
        res += computeInset(t, f, input, axis, f->padding->bottom);

        if (f->borders->top.style != BorderStyle::NONE)
            res += computeInset(t, f, axis, f->borders->top.width);

        if (f->borders->bottom.style != BorderStyle::NONE)
            res += computeInset(t, f, axis, f->borders->bottom.width);
    }

    return res;
}

// https://www.w3.org/TR/css-sizing-3/#outer-size
Px computePreferredOuterSize(Tree &t, Frag &f, Input input, Axis axis, Px availableSpace) {
    auto res = computePreferredBorderSize(t, f, input, axis, availableSpace);

    if (axis == Axis::HORIZONTAL) {
        res += computeInset(t, f, input, axis, f->margin->start);
        res += computeInset(t, f, input, axis, f->margin->end);
    } else {
        res += computeInset(t, f, input, axis, f->margin->top);
        res += computeInset(t, f, input, axis, f->margin->bottom);
    }

    return res;
}

// MARK: Box -------------------------------------------------------------------

Box computeBox(Tree &t, Frag &f, Input input, RectPx borderBox) {
    Box res;

    res.paddings = computePadding(t, f, input);
    res.borders = computeBorders(t, f);
    res.radii = computeRadii(t, f, input);
    borderBox.height = max(
        borderBox.height,
        res.paddings.all().height + res.borders.all().height
    );
    borderBox.width = max(
        borderBox.width,
        res.paddings.all().width + res.borders.all().width
    );
    res.borderBox = borderBox;
    res.margins = computeMargins(t, f, input);

    return res;
}

} // namespace Vaev::Layout
