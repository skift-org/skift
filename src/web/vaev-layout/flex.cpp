#include "flex.h"

#include "sizing.h"

namespace Vaev::Layout {

void FlexFlow::_clear() {
    _items.clear();
    _lines.clear();
}

void FlexFlow::_createItems() {
    for (usize i = 0; i < _frags.len(); i++)
        _items.pushBack({i});
}

void FlexFlow::_sortByOrder() {
    stableSort(_items, [&](auto a, auto b) {
        return styleAt(a.frag).order < styleAt(b.frag).order;
    });
}

void FlexFlow::placeChildren(Context &ctx, Box box) {
    Frag::placeChildren(ctx, box);

    _clear();
    _createItems();
    _sortByOrder();

    Axis mainAxis = Axis::HORIZONTAL;

    Px res = box.contentBox().start();
    auto blockSize = computePreferredBorderSize(
        ctx,
        mainAxis.cross(),
        box.contentBox().height
    );

    for (auto &c : _frags) {
        auto childcontext = ctx.subContext(
            *c,
            mainAxis,
            box.contentBox()
        );

        auto inlineSize = computePreferredOuterSize(
            childcontext, mainAxis,
            max(Px{0}, box.contentBox().width - res)
        );

        RectPx borderBox = RectPx{
            res,
            box.contentBox().top(),
            inlineSize,
            blockSize,
        };

        auto box = computeBox(childcontext, borderBox);
        c->placeChildren(childcontext, box);

        res += inlineSize;
    }
}

Px FlexFlow::computeIntrinsicSize(Context &ctx, Axis axis, IntrinsicSize intrinsic, Px) {
    Px res = Px{};

    for (auto &c : _frags) {
        auto childCtx = ctx.subContext(
            *c,
            axis,
            Vec2Px::ZERO
        );

        if (axis == Axis::HORIZONTAL) {
            auto size = computePreferredOuterSize(childCtx, axis);
            if (intrinsic == IntrinsicSize::MAX_CONTENT) {
                res += size;
            } else {
                res = max(res, size);
            }
        } else {
            auto size = computePreferredOuterSize(childCtx, axis);
            res = max(res, size);
        }
    }

    return res;
}

} // namespace Vaev::Layout
