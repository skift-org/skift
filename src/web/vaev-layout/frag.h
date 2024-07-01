#pragma once

#include <karm-base/rc.h>
#include <karm-base/vec.h>
#include <karm-logger/logger.h>
#include <vaev-base/length.h>
#include <vaev-paint/base.h>
#include <vaev-paint/box.h>
#include <vaev-paint/stack.h>
#include <vaev-style/computed.h>

namespace Vaev::Layout {

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

    RectPx _borderBox;
    SpacingPx _BorderPadding;
    SpacingPx _margin;

    Frag(Strong<Style::Computed> style)
        : _style(style) {
    }

    virtual ~Frag() = default;
    virtual Type type() const = 0;

    virtual void layout(RectPx bound) {
        _borderBox = bound;
    }

    virtual void paint(Paint::Stack &stack) {
        if (style().backgrounds.len()) {
            Paint::Box box;
            box.backgrounds = style().backgrounds;
            box.bound = _borderBox;
            stack.add(makeStrong<Paint::Box>(std::move(box)));
        }
    }

    virtual void debug(Gfx::Context &g) {
        g.stroke(_borderBox.cast<isize>());
    }

    virtual void repr(Io::Emit &e) const {
        e("({} {})", type(), _borderBox);
    }

    virtual Px contentInlineSize() const {
        return Px{100};
    }

    virtual Px contentBlockSize() const {
        return Px{100};
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
