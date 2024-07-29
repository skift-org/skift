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

        res += computeInset(ctx, axis, style.borders->start.width);
        res += computeInset(ctx, axis, style.borders->end.width);

        res += computeInset(ctx, axis, style.margin->start);
        res += computeInset(ctx, axis, style.margin->end);
    } else {
        res += computeInset(ctx, axis, style.padding->top);
        res += computeInset(ctx, axis, style.padding->bottom);

        res += computeInset(ctx, axis, style.borders->top.width);
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

        res += max(Px{0}, computeInset(ctx, axis, style.borders->start.width));
        res += max(Px{0}, computeInset(ctx, axis, style.borders->end.width));
    } else {
        res += max(Px{0}, computeInset(ctx, axis, style.padding->top));
        res += max(Px{0}, computeInset(ctx, axis, style.padding->bottom));

        res += max(Px{0}, computeInset(ctx, axis, style.borders->top.width));
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

        res += computeInset(ctx, axis, style.borders->start.width);
        res += computeInset(ctx, axis, style.borders->end.width);
    } else {
        res += computeInset(ctx, axis, style.padding->top);
        res += computeInset(ctx, axis, style.padding->bottom);

        res += computeInset(ctx, axis, style.borders->top.width);
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

    res.borders.top = computeInset(ctx, Axis::VERTICAL, ctx.style().borders->top.width);
    res.borders.end = computeInset(ctx, Axis::HORIZONTAL, ctx.style().borders->end.width);
    res.borders.bottom = computeInset(ctx, Axis::VERTICAL, ctx.style().borders->bottom.width);
    res.borders.start = computeInset(ctx, Axis::HORIZONTAL, ctx.style().borders->start.width);

    res.borderBox = borderBox;

    res.margins.top = computeInset(ctx, Axis::VERTICAL, ctx.style().margin->top);
    res.margins.end = computeInset(ctx, Axis::HORIZONTAL, ctx.style().margin->end);
    res.margins.bottom = computeInset(ctx, Axis::VERTICAL, ctx.style().margin->bottom);
    res.margins.start = computeInset(ctx, Axis::HORIZONTAL, ctx.style().margin->start);

    return res;
}

} // namespace Vaev::Layout
