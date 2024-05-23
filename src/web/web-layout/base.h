#pragma once

#include <karm-base/rc.h>
#include <karm-base/vec.h>
#include <web-css/computed-values.h>
#include <web-unit/length.h>

namespace Web::Layout {

struct Frag {
    enum struct Type {
        BLOCK,
        FLEX,
        GRID,
        IMAGE,
        INLINE,
        TABLE,
        TEXT
    };

    using enum Type;

    Unit::RectPx borderBox;
    Unit::SpacingPx BorderPadding;
    Unit::SpacingPx margin;
    Strong<Css::ComputedValues> computedValues;

    virtual ~Frag() = default;
    virtual Type type() const = 0;
    virtual void layout() = 0;
    virtual void size() = 0;

    template <typename T>
    T *is() {
        return type() == T::TYPE ? static_cast<T *>(this) : nullptr;
    }

    template <typename T>
    T const *is() const {
        return type() == T::TYPE ? static_cast<T const *>(this) : nullptr;
    }
};

struct Flow : public Frag {
    Vec<Strong<Frag>> children;
};

} // namespace Web::Layout
