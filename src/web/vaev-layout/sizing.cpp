#include "sizing.h"

// https://www.w3.org/TR/css-box-4
// https://www.w3.org/TR/css-sizing-3

namespace Vaev::Layout {

// MARK: Insets ----------------------------------------------------------------

Px computeInset(Context &ctx, Axis, Length inset) {
    return ctx.resolve(inset);
}

Px computeInset(Context &ctx, Axis axis, PercentOr<Length> inset) {
    return ctx.resolve(inset, ctx.containingBlock.wh[axis.index()]);
}

Px computeInset(Context &ctx, Axis axis, Width inset) {
    return ctx.resolve(inset, ctx.containingBlock.wh[axis.index()]);
}

Px allInsets(Context &ctx, Axis axis) {
    Px res = Px{0};
    auto style = ctx.style();
    if (axis == Axis::HORIZONTAL) {
        res += computeInset(ctx, axis, style.padding->start);
        res += computeInset(ctx, axis, style.padding->end);

        if (style.borders->start.style != BorderStyle::NONE)
            res += computeInset(ctx, axis, style.borders->start.width);

        if (style.borders->end.style != BorderStyle::NONE)
            res += computeInset(ctx, axis, style.borders->end.width);

        res += computeInset(ctx, axis, style.margin->start);
        res += computeInset(ctx, axis, style.margin->end);
    } else {
        res += computeInset(ctx, axis, style.padding->top);
        res += computeInset(ctx, axis, style.padding->bottom);

        if (style.borders->top.style != BorderStyle::NONE)
            res += computeInset(ctx, axis, style.borders->top.width);

        if (style.borders->bottom.style != BorderStyle::NONE)
            res += computeInset(ctx, axis, style.borders->bottom.width);

        res += computeInset(ctx, axis, style.margin->top);
        res += computeInset(ctx, axis, style.margin->bottom);
    }

    return res;
}

// MARK: Content ---------------------------------------------------------------

Px computeSpecifiedSize(Context &ctx, Axis axis, Size size, Px availableSpace) {
    Px res = Px{0};

    availableSpace = max(Px{0}, availableSpace - allInsets(ctx, axis));

    if (size == Size::MIN_CONTENT) {
        res = ctx.frag.computeIntrinsicSize(
            ctx,
            axis,
            IntrinsicSize::MIN_CONTENT,
            availableSpace
        );
    } else if (size == Size::MAX_CONTENT) {
        res = ctx.frag.computeIntrinsicSize(
            ctx,
            axis,
            IntrinsicSize::MAX_CONTENT,
            Px{0}
        );
    } else if (size == Size::AUTO) {
        res = ctx.frag.computeIntrinsicSize(
            ctx,
            axis,
            IntrinsicSize::MAX_CONTENT,
            availableSpace
        );
    } else if (size == Size::FIT_CONTENT) {
        res = ctx.frag.computeIntrinsicSize(
            ctx,
            axis,
            IntrinsicSize::MAX_CONTENT,
            ctx.resolve(size.value, ctx.containingBlock.wh[axis.index()])
        );
    } else if (size == Size::LENGTH) {
        res = ctx.resolve(size.value, ctx.containingBlock.width);
    } else {
        logWarn("unknown specified size: {}", size);
        res = Px{0};
    }

    auto &style = ctx.style();

    if (style.boxSizing == BoxSizing::CONTENT_BOX) {
        return res;
    }

    if (axis == Axis::HORIZONTAL) {
        res += max(Px{0}, computeInset(ctx, axis, style.padding->start));
        res += max(Px{0}, computeInset(ctx, axis, style.padding->end));

        if (style.borders->start.style != BorderStyle::NONE)
            res += max(Px{0}, computeInset(ctx, axis, style.borders->start.width));

        if (style.borders->end.style != BorderStyle::NONE)
            res += max(Px{0}, computeInset(ctx, axis, style.borders->end.width));
    } else {
        res += max(Px{0}, computeInset(ctx, axis, style.padding->top));
        res += max(Px{0}, computeInset(ctx, axis, style.padding->bottom));

        if (style.borders->top.style != BorderStyle::NONE)
            res += max(Px{0}, computeInset(ctx, axis, style.borders->top.width));

        if (style.borders->bottom.style != BorderStyle::NONE)
            res += max(Px{0}, computeInset(ctx, axis, style.borders->bottom.width));
    }

    return res;
}

// https://www.w3.org/TR/css-sizing-3/#preferred-size-properties
Px computePreferredSize(Context &ctx, Axis axis, Px availableSpace) {
    auto &style = ctx.style();
    auto specifiedSize = style.sizing->size(axis);

    Px res = computeSpecifiedSize(ctx, axis, specifiedSize, availableSpace);

    auto minSize = style.sizing->minSize(axis);
    if (minSize != Size::AUTO) {
        res = max(res, computeSpecifiedSize(ctx, axis, minSize, availableSpace));
    }

    auto maxSize = style.sizing->maxSize(axis);
    if (maxSize != Size::NONE) {
        res = min(res, computeSpecifiedSize(ctx, axis, maxSize, availableSpace));
    }

    return res;
}

Px computePreferredContentSize(Context &ctx, Axis axis, Px availableSpace) {
    auto &style = ctx.style();
    auto specifiedSize = style.sizing->size(axis);

    return computeSpecifiedSize(ctx, axis, specifiedSize, availableSpace);
}

// https://www.w3.org/TR/css-box-4/#border-box
Px computePreferredBorderSize(Context &ctx, Axis axis, Px availableSpace) {
    auto &style = ctx.frag.style();

    auto res = computePreferredSize(ctx, axis, availableSpace);

    if (style.boxSizing == BoxSizing::BORDER_BOX)
        return res;

    if (axis == Axis::HORIZONTAL) {
        res += computeInset(ctx, axis, style.padding->start);
        res += computeInset(ctx, axis, style.padding->end);

        if (style.borders->start.style != BorderStyle::NONE)
            res += computeInset(ctx, axis, style.borders->start.width);

        if (style.borders->end.style != BorderStyle::NONE)
            res += computeInset(ctx, axis, style.borders->end.width);
    } else {
        res += computeInset(ctx, axis, style.padding->top);
        res += computeInset(ctx, axis, style.padding->bottom);

        if (style.borders->top.style != BorderStyle::NONE)
            res += computeInset(ctx, axis, style.borders->top.width);

        if (style.borders->bottom.style != BorderStyle::NONE)
            res += computeInset(ctx, axis, style.borders->bottom.width);
    }

    return res;
}

// https://www.w3.org/TR/css-sizing-3/#outer-size
Px computePreferredOuterSize(Context &ctx, Axis axis, Px availableSpace) {
    auto &style = ctx.style();
    auto res = computePreferredBorderSize(ctx, axis, availableSpace);

    if (axis == Axis::HORIZONTAL) {
        res += computeInset(ctx, axis, style.margin->start);
        res += computeInset(ctx, axis, style.margin->end);
    } else {
        res += computeInset(ctx, axis, style.margin->top);
        res += computeInset(ctx, axis, style.margin->bottom);
    }

    return res;
}

// MARK: Box -------------------------------------------------------------------

Box computeBox(Context &ctx, RectPx borderBox) {
    Box res;

    res.paddings.top = computeInset(ctx, Axis::VERTICAL, ctx.style().padding->top);
    res.paddings.end = computeInset(ctx, Axis::HORIZONTAL, ctx.style().padding->end);
    res.paddings.bottom = computeInset(ctx, Axis::VERTICAL, ctx.style().padding->bottom);
    res.paddings.start = computeInset(ctx, Axis::HORIZONTAL, ctx.style().padding->start);

    if (ctx.style().borders->top.style != BorderStyle::NONE)
        res.borders.top = computeInset(ctx, Axis::VERTICAL, ctx.style().borders->top.width);

    if (ctx.style().borders->end.style != BorderStyle::NONE)
        res.borders.end = computeInset(ctx, Axis::HORIZONTAL, ctx.style().borders->end.width);

    if (ctx.style().borders->bottom.style != BorderStyle::NONE)
        res.borders.bottom = computeInset(ctx, Axis::VERTICAL, ctx.style().borders->bottom.width);

    if (ctx.style().borders->start.style != BorderStyle::NONE)
        res.borders.start = computeInset(ctx, Axis::HORIZONTAL, ctx.style().borders->start.width);

    res.radii.a = computeInset(ctx, Axis::VERTICAL, ctx.style().borders->radii.a);
    res.radii.b = computeInset(ctx, Axis::HORIZONTAL, ctx.style().borders->radii.b);
    res.radii.c = computeInset(ctx, Axis::HORIZONTAL, ctx.style().borders->radii.c);
    res.radii.d = computeInset(ctx, Axis::VERTICAL, ctx.style().borders->radii.d);
    res.radii.e = computeInset(ctx, Axis::VERTICAL, ctx.style().borders->radii.e);
    res.radii.f = computeInset(ctx, Axis::HORIZONTAL, ctx.style().borders->radii.f);
    res.radii.g = computeInset(ctx, Axis::HORIZONTAL, ctx.style().borders->radii.g);
    res.radii.h = computeInset(ctx, Axis::VERTICAL, ctx.style().borders->radii.h);

    res.borderBox = borderBox;

    res.margins.top = computeInset(ctx, Axis::VERTICAL, ctx.style().margin->top);
    res.margins.end = computeInset(ctx, Axis::HORIZONTAL, ctx.style().margin->end);
    res.margins.bottom = computeInset(ctx, Axis::VERTICAL, ctx.style().margin->bottom);
    res.margins.start = computeInset(ctx, Axis::HORIZONTAL, ctx.style().margin->start);

    return res;
}

} // namespace Vaev::Layout
