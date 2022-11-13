#pragma once

#include "box.h"
#include "funcs.h"
#include "view.h"

namespace Karm::Ui {

template <typename T>
using OnChange = Opt<Func<void(Node &, T value)>>;

/* --- Button --------------------------------------------------------------- */

struct MouseListener {
    enum MouseState {
        IDLE,
        HOVER,
        PRESS,
    };

    MouseState _state = IDLE;

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

    bool listen(Node &node, Events::Event &e) {
        bool result = false;
        MouseState state = _state;

        e.handle<Events::MouseEvent>([&](auto &m) {
            if (!node.bound().contains(m.pos)) {
                state = IDLE;
                return false;
            }

            if (state != PRESS) {
                state = HOVER;
            }

            if (m.type == Events::MouseEvent::PRESS) {
                state = PRESS;
                return true;
            } else if (m.type == Events::MouseEvent::RELEASE) {
                state = HOVER;
                result = true;
                return true;
            }

            return false;
        });

        if (state != _state) {
            _state = state;
            shouldRepaint(node);
        }

        return result;
    }
};

struct ButtonStyle {
    static constexpr int RADIUS = 4;

    BoxStyle idleStyle;
    BoxStyle hoverStyle;
    BoxStyle pressStyle;
    BoxStyle disabledStyle = {
        .foregroundColor = Gfx::ZINC600,
    };

    static ButtonStyle regular();

    static ButtonStyle secondary();

    static ButtonStyle primary();

    static ButtonStyle outline();

    static ButtonStyle subtle();

    static ButtonStyle destructive();

    ButtonStyle withRadius(Gfx::BorderRadius radius) const;

    ButtonStyle withForegroundColor(Gfx::Color color) const;
};

using OnPress = Opt<Func<void(Node &)>>;

Child button(OnPress onPress, ButtonStyle style, Child child);

Child button(OnPress onPress, ButtonStyle style, Str t);

Child button(OnPress onPress, ButtonStyle style, Media::Icon i);

Child button(OnPress onPress, ButtonStyle style, Media::Icon i, Str t);

Child button(OnPress onPress, Child child);

Child button(OnPress onPress, Str t);

Child button(OnPress onPress, Media::Icons i);

Child button(OnPress onPress, Media::Icons i, Str t);

/* --- Input ---------------------------------------------------------------- */

Child input(TextStyle style, String text, OnChange<String> onChange = NONE);

Child input(String text, OnChange<String> onChange = NONE);

/* --- Toggle --------------------------------------------------------------- */

Child toggle(bool value, OnChange<bool> onChange);

/* --- Checkbox ------------------------------------------------------------- */

Child checkbox(bool value, OnChange<bool> onChange);

/* --- Radio ---------------------------------------------------------------- */

Child radio(bool value, OnChange<bool> onChange);

/* --- Slider --------------------------------------------------------------- */

/* --- Select --------------------------------------------------------------- */

} // namespace Karm::Ui
