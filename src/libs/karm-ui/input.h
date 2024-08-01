#pragma once

#include <karm-text/edit.h>

#include "box.h"
#include "funcs.h"
#include "view.h"

namespace Karm::Ui {

template <typename T = None>
using OnChange = Meta::Cond<
    Meta::Same<T, None>,
    Opt<SharedFunc<void(Node &)>>,
    Opt<SharedFunc<void(Node &, T value)>>>;

template <typename T>
[[gnu::used]] static auto IGNORE(Ui::Node &, T) {}

// MARK: Button ----------------------------------------------------------------

struct MouseListener {
    enum MouseState {
        IDLE,
        HOVER,
        PRESS,
    };

    MouseState _state = IDLE;
    Math::Vec2i _pos = {0, 0};

    auto state() const {
        return _state;
    }

    bool isIdle() const {
        return _state == IDLE;
    }

    bool isHover() const {
        return _state == HOVER;
    }

    bool isPress() const {
        return _state == PRESS;
    }

    auto pos() const {
        return _pos;
    }

    bool listen(Node &node, Sys::Event &event) {
        bool result = false;
        MouseState state = _state;

        if (auto *e = event.is<Events::MouseEvent>()) {
            if (not node.bound().contains(e->pos)) {
                state = IDLE;
            } else {

                if (state != PRESS) {
                    state = HOVER;
                }

                _pos = e->pos - node.bound().topStart();

                if (e->type == Events::MouseEvent::PRESS and
                    e->button == Events::Button::LEFT) {
                    state = PRESS;
                    event.accept();

                } else if (e->type == Events::MouseEvent::RELEASE and
                           e->button == Events::Button::LEFT) {
                    state = HOVER;
                    result = true;
                    event.accept();
                }
            }
        } else if (auto *e = event.is<Events::MouseLeaveEvent>()) {
            state = IDLE;
        }

        if (state != _state) {
            _state = state;
            shouldRepaint(node);
        }

        return result;
    }
};

struct ButtonStyle {
    static constexpr isize RADIUS = 4;

    BoxStyle idleStyle{};
    BoxStyle hoverStyle{};
    BoxStyle pressStyle{};
    BoxStyle disabledStyle = {
        .foregroundPaint = GRAY600,
    };

    static ButtonStyle none();

    static ButtonStyle regular(Gfx::ColorRamp ramp = GRAYS);

    static ButtonStyle secondary();

    static ButtonStyle primary();

    static ButtonStyle outline();

    static ButtonStyle subtle();

    static ButtonStyle text();

    static ButtonStyle destructive();

    ButtonStyle withRadii(Math::Radiif radii) const;

    ButtonStyle withForegroundPaint(Gfx::Paint paint) const;

    ButtonStyle withPadding(Math::Spacingi spacing) const;

    ButtonStyle withMargin(Math::Spacingi spacing) const;
};

using OnPress = Opt<Func<void(Node &)>>;

[[gnu::used]] static auto NOP(Ui::Node &) {}

Child button(OnPress onPress, ButtonStyle style, Child child);

inline auto button(OnPress onPress, ButtonStyle style) {
    return [onPress = std::move(onPress), style](Child child) mutable {
        return button(std::move(onPress), style, child);
    };
}

Child button(OnPress onPress, ButtonStyle style, Str t);

Child button(OnPress onPress, ButtonStyle style, Media::Icon i);

Child button(OnPress onPress, ButtonStyle style, Media::Icon i, Str t);

Child button(OnPress onPress, Child child);

inline auto button(OnPress onPress) {
    return [onPress = std::move(onPress)](Child child) mutable {
        return button(std::move(onPress), child);
    };
}

Child button(OnPress onPress, Str t);

Child button(OnPress onPress, Mdi::Icon i);

Child button(OnPress onPress, Mdi::Icon i, Str t);

// MARK: Input -----------------------------------------------------------------

Child input(Text::ProseStyle style, Strong<Text::Model> text, OnChange<Text::Action> onChange);

Child input(Strong<Text::Model> text, OnChange<Text::Action> onChange);

// MARK: Slider ----------------------------------------------------------------

Child slider(f64 value, OnChange<f64> onChange, Child child);

static inline auto slider(f64 value, OnChange<f64> onChange) {
    return [value, onChange = std::move(onChange)](Child child) mutable {
        return slider(value, std::move(onChange), std::move(child));
    };
}

// MARK: Intent ----------------------------------------------------------------

using Filter = Func<void(Node &, Sys::Event &e)>;

Child intent(Filter map, Child child);

static inline auto intent(Filter filter) {
    return [filter = std::move(filter)](Child child) mutable {
        return intent(std::move(filter), std::move(child));
    };
}

} // namespace Karm::Ui
