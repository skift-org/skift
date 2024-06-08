#pragma once

#include <karm-base/rc.h>
#include <karm-base/vec.h>
#include <web-cssom/computed.h>
#include <web-types/length.h>

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

    Strong<CSSOM::Computed> _style;

    Types::RectPx _borderBox;
    Types::SpacingPx _BorderPadding;
    Types::SpacingPx _margin;

    Frag(Strong<CSSOM::Computed> style)
        : _style(style) {
    }

    virtual ~Frag() = default;
    virtual Type type() const = 0;

    virtual void layout(Types::RectPx) = 0;
    virtual Types::Vec2Px size(Types::Vec2Px) = 0;

    CSSOM::Computed const &style() const {
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

    Frag &fragAt(usize frag) {
        return *_frags[frag];
    }

    Frag const &fragAt(usize frag) const {
        return *_frags[frag];
    }

    CSSOM::Computed const &styleAt(usize frag) const {
        return _frags[frag]->style();
    }
};

} // namespace Web::Layout
