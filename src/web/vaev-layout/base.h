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
    Str typeStr() const {
        switch (type()) {
#define ITER(NAME)   \
    case Type::NAME: \
        return #NAME;
            FOREACH_TYPE(ITER)
#undef ITER
        }
    }

    virtual void layout(RectPx bound) {
        logDebug("setting border box: {}", bound);
        _borderBox = bound;
    }

    virtual void paint(Paint::Stack &stack) {
        if (style().backgrounds.len()) {
            Paint::Box box;
            box.backgrounds = style().backgrounds;
            box.bound.wh = {Px{100}, Px{100}};
            stack.add(makeStrong<Paint::Box>(std::move(box)));
        }
    }

    virtual void repr(Io::Emit &e) const {
        e("({} {})", typeStr(), _borderBox);
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
        logDebug("layout: {}", bound);
        Frag::layout(bound);
        for (auto &c : _frags) {
            c->layout(bound);
        }
    }

    void paint(Paint::Stack &stack) override {
        Frag::paint(stack);

        for (auto &c : _frags) {
            c->paint(stack);
        }
    }

    void repr(Io::Emit &e) const override {
        e("({}", typeStr());
        if (_frags) {
            e.indentNewline();
            for (auto &c : _frags) {
                c->repr(e);
                e.newline();
            }
            e.deindent();
        }
        e(")");
    }
};

#undef FOREACH_TYPE

} // namespace Vaev::Layout
