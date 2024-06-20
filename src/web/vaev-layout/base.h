#pragma once

#include <karm-base/rc.h>
#include <karm-base/vec.h>
#include <vaev-base/length.h>
#include <vaev-paint/base.h>
#include <vaev-style/computed.h>

namespace Vaev::Layout {

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

    Strong<Style::Computed> _style;

    RectPx _borderBox;
    SpacingPx _BorderPadding;
    SpacingPx _margin;

    Frag(Strong<Style::Computed> style)
        : _style(style) {
    }

    virtual ~Frag() = default;
    virtual Type type() const = 0;

    virtual void layout(RectPx) = 0;
    virtual Vec2Px size(Vec2Px) = 0;
    virtual void paint(Paint::Node &) = 0;

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

struct Flow : public Frag {
    using Frag::Frag;

    Vec<Strong<Frag>> _frags;

    Frag &fragAt(usize frag) {
        return *_frags[frag];
    }

    Frag const &fragAt(usize frag) const {
        return *_frags[frag];
    }

    Style::Computed const &styleAt(usize frag) const {
        return _frags[frag]->style();
    }

    void add(Strong<Frag> frag) {
        _frags.pushBack(frag);
    }

    void layout(RectPx bound) override {
        for (auto &c : _frags) {
            c->layout(bound);
        }
    }

    void paint(Paint::Node &node) override {
        for (auto &c : _frags) {
            c->paint(node);
        }
    }
};

} // namespace Vaev::Layout
