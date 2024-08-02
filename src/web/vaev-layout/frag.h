#pragma once

#include <karm-base/rc.h>
#include <karm-base/vec.h>
#include <karm-logger/logger.h>
#include <vaev-base/length.h>
#include <vaev-paint/base.h>
#include <vaev-paint/box.h>
#include <vaev-paint/stack.h>
#include <vaev-style/computed.h>

#include "box.h"
#include "context.h"

namespace Vaev::Layout {

enum struct IntrinsicSize {
    MIN_CONTENT,
    MAX_CONTENT,
};

#define FOREACH_TYPE(ITER) \
    ITER(BLOCK)            \
    ITER(FLEX)             \
    ITER(GRID)             \
    ITER(IMAGE)            \
    ITER(INLINE)           \
    ITER(TABLE)            \
    ITER(RUN)

struct Frag {
    enum struct Type {
#define ITER(NAME) NAME,
        FOREACH_TYPE(ITER)
#undef ITER

            _LEN,
    };

    using enum Type;

    Strong<Style::Computed> _style;
    Box _box;

    Frag(Strong<Style::Computed> style)
        : _style(style) {
    }

    virtual ~Frag() = default;

    virtual Type type() const = 0;

    virtual void placeChildren(Context &, Box box) {
        _box = box;
    }

    virtual Px computeIntrinsicSize(Context &, Axis, IntrinsicSize, Px availableSpace) {
        (void)availableSpace;
        return Px{100};
    }

    virtual void makePaintables(Paint::Stack &stack) {
        if (style().backgrounds.len()) {
            Paint::Box box;
            ColorContext colorContext;
            // FIXME colorContext should be context dependant

            box.borders = Paint::Borders();
            box.borders.radii = _box.radius.cast<f64>();

            box.borders.top.width = _box.borders.top.toFloat<f64>();
            box.borders.top.style = style().borders->top.style;
            box.borders.top.fill = colorContext.resolve(style().borders->top.color);

            box.borders.bottom.width = _box.borders.bottom.toFloat<f64>();
            box.borders.bottom.style = style().borders->bottom.style;
            box.borders.bottom.fill = colorContext.resolve(style().borders->bottom.color);

            box.borders.start.width = _box.borders.start.toFloat<f64>();
            box.borders.start.style = style().borders->start.style;
            box.borders.start.fill = colorContext.resolve(style().borders->start.color);

            box.borders.end.width = _box.borders.end.toFloat<f64>();
            box.borders.end.style = style().borders->end.style;
            box.borders.end.fill = colorContext.resolve(style().borders->end.color);

            box.backgrounds = style().backgrounds;
            box.bound = _box.borderBox;
            stack.add(makeStrong<Paint::Box>(std::move(box)));
        }
    }

    virtual void paintWireframe(Gfx::Context &g) {
        g.strokeStyle({
            .fill = Gfx::GREEN,
            .width = 1,
            .align = Gfx::INSIDE_ALIGN,
        });
        g.stroke(_box.borderBox.cast<f64>());
    }

    virtual void repr(Io::Emit &e) const {
        e("({} {})", type(), _box);
    }

    Style::Computed const &style() const {
        return *_style;
    }

    template <typename T>
    T *is() {
        return type() == T::TYPE ? static_cast<T *>(this) : nullptr;
    }

    template <typename T>
    T const *is() const {
        return type() == T::TYPE ? static_cast<T const *>(this) : nullptr;
    }
};

#undef FOREACH_TYPE

} // namespace Vaev::Layout
