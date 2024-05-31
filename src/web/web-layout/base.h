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
        RUN
    };

    using enum Type;

    Strong<Css::ComputedValues> _style;

    Unit::RectPx _borderBox;
    Unit::SpacingPx _BorderPadding;
    Unit::SpacingPx _margin;

    Frag(Strong<Css::ComputedValues> style)
        : _style(style) {
    }

    virtual ~Frag() = default;
    virtual Type type() const = 0;

    virtual void layout(Unit::RectPx) = 0;
    virtual Unit::Vec2Px size(Unit::Vec2Px) = 0;

    Css::ComputedValues const &style() const {
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

struct Flow : public Frag {
    using Frag::Frag;

    Vec<Strong<Frag>> _frags;

    Frag &frag(usize frag) {
        return *_frags[frag];
    }

    Frag const &frag(usize frag) const {
        return *_frags[frag];
    }

    Css::ComputedValues const &style(usize frag) const {
        return _frags[frag]->style();
    }
};

} // namespace Web::Layout
