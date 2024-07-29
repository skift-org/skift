#pragma once

#include "flow.h"
#include "sizing.h"

namespace Vaev::Layout {

struct FlexFlow : public Flow {
    static constexpr auto TYPE = FLEX;

    struct Item {
        usize frag;
    };

    struct Line {
        urange items;
    };

    Vec<Item> _items;
    Vec<Line> _lines;

    using Flow::Flow;

    Type type() const override {
        return TYPE;
    }

    void _clear() {
        _items.clear();
        _lines.clear();
    }

    void _createItems() {
        for (usize i = 0; i < _frags.len(); i++)
            _items.pushBack({i});
    }

    void _sortByOrder() {
        stableSort(_items, [&](auto a, auto b) {
            return styleAt(a.frag).order < styleAt(b.frag).order;
        });
    }

    void placeChildren(Context &ctx, Box box) override {
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

    Px computeIntrinsicSize(Context &ctx, Axis axis, IntrinsicSize intrinsic, Px) override {
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
};

} // namespace Vaev::Layout
