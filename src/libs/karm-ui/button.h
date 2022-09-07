#pragma once

#include "align.h"
#include "box.h"
#include "flow.h"
#include "funcs.h"
#include "icon.h"
#include "text.h"

namespace Karm::Ui {
struct ButtonStyle {
    BoxStyle idleStyle;
    BoxStyle hoverStyle;
    BoxStyle pressStyle;
};

enum ButtonState {
    IDLE,
    OVER,
    PRESS,
};

struct Button : public _Box<Button> {
    static constexpr int RADIUS = 4;

    static constexpr ButtonStyle DEFAULT = {
        .idleStyle = {
            .borderRadius = RADIUS,
            .backgroundColor = Gfx::ZINC700,
        },
        .hoverStyle = {
            .borderRadius = RADIUS,
            .borderWidth = 1,
            .backgroundColor = Gfx::ZINC600,
        },
        .pressStyle = {
            .borderRadius = RADIUS,
            .borderWidth = 1,
            .borderColor = Gfx::ZINC600,
            .backgroundColor = Gfx::ZINC700,
        },
    };

    static constexpr ButtonStyle DEFAULT_ROUND = {
        .idleStyle = {
            .borderRadius = 999,
            .backgroundColor = Gfx::ZINC700,
        },
        .hoverStyle = {
            .borderRadius = 999,
            .borderWidth = 1,
            .backgroundColor = Gfx::ZINC600,
        },
        .pressStyle = {
            .borderRadius = 999,
            .borderWidth = 1,
            .borderColor = Gfx::ZINC600,
            .backgroundColor = Gfx::ZINC700,
        },
    };

    static constexpr ButtonStyle PRIMARY = {
        .idleStyle = {
            .borderRadius = RADIUS,
            .backgroundColor = Gfx::BLUE700,
        },
        .hoverStyle = {
            .borderRadius = RADIUS,
            .borderWidth = 1,
            .backgroundColor = Gfx::BLUE600,
        },
        .pressStyle = {
            .borderRadius = RADIUS,
            .borderWidth = 1,
            .borderColor = Gfx::BLUE600,
            .backgroundColor = Gfx::BLUE700,
        },
    };

    static constexpr ButtonStyle PRIMARY_ROUND = {
        .idleStyle = {
            .borderRadius = 999,
            .backgroundColor = Gfx::BLUE700,
        },
        .hoverStyle = {
            .borderRadius = 999,
            .borderWidth = 1,
            .backgroundColor = Gfx::BLUE600,
        },
        .pressStyle = {
            .borderRadius = 999,
            .borderWidth = 1,
            .borderColor = Gfx::BLUE600,
            .backgroundColor = Gfx::BLUE700,
        },
    };

    static constexpr ButtonStyle OUTLINE = {
        .idleStyle = {
            .borderRadius = RADIUS,
            .borderWidth = 1,
            .borderColor = Gfx::ZINC700,
        },
        .hoverStyle = {
            .borderRadius = RADIUS,
            .borderWidth = 1,
            .backgroundColor = Gfx::ZINC600,
        },
        .pressStyle = {
            .borderRadius = RADIUS,
            .borderWidth = 1,
            .borderColor = Gfx::ZINC600,
            .backgroundColor = Gfx::ZINC700,
        },
    };

    static constexpr ButtonStyle OUTLINE_ROUND = {
        .idleStyle = {
            .borderRadius = 999,
            .borderWidth = 1,
            .borderColor = Gfx::ZINC700,
        },
        .hoverStyle = {
            .borderRadius = 999,
            .borderWidth = 1,
            .backgroundColor = Gfx::ZINC600,
        },
        .pressStyle = {
            .borderRadius = 999,
            .borderWidth = 1,
            .borderColor = Gfx::ZINC600,
            .backgroundColor = Gfx::ZINC700,
        },
    };

    static constexpr ButtonStyle SUBTLE = {
        .idleStyle = {},
        .hoverStyle = {
            .borderRadius = RADIUS,
            .borderWidth = 1,
            .backgroundColor = Gfx::ZINC600,
        },
        .pressStyle = {
            .borderRadius = RADIUS,
            .borderWidth = 1,
            .borderColor = Gfx::ZINC600,
            .backgroundColor = Gfx::ZINC700,
        },
    };

    static constexpr ButtonStyle SUBTLE_ICON = {
        .idleStyle = {
            .foregroundColor = Gfx::ZINC400,
        },
        .hoverStyle = {
            .borderRadius = RADIUS,
            .borderWidth = 1,
            .backgroundColor = Gfx::ZINC600,
        },
        .pressStyle = {
            .borderRadius = RADIUS,
            .borderWidth = 1,
            .borderColor = Gfx::ZINC600,
            .backgroundColor = Gfx::ZINC700,
        },
    };

    static constexpr ButtonStyle SUBTLE_ROUND = {
        .idleStyle = {
            .foregroundColor = Gfx::ZINC400,
        },
        .hoverStyle = {
            .borderRadius = 999,
            .borderWidth = 1,
            .backgroundColor = Gfx::ZINC600,
        },
        .pressStyle = {
            .borderRadius = 999,
            .borderWidth = 1,
            .borderColor = Gfx::ZINC600,
            .backgroundColor = Gfx::ZINC700,
        },
    };

    static constexpr ButtonStyle DESTRUCTIVE = {
        .idleStyle = {
            .borderRadius = RADIUS,
            .foregroundColor = Gfx::RED500,
        },
        .hoverStyle = {
            .borderRadius = RADIUS,
            .borderWidth = 1,
            .backgroundColor = Gfx::RED600,
        },
        .pressStyle = {
            .borderRadius = RADIUS,
            .borderWidth = 1,
            .borderColor = Gfx::RED600,
            .backgroundColor = Gfx::RED700,
        },
    };

    static constexpr ButtonStyle DESTRUCTIVE_ICON = {
        .idleStyle = {
            .borderRadius = RADIUS,
            .foregroundColor = Gfx::ZINC400,
        },
        .hoverStyle = {
            .borderRadius = RADIUS,
            .borderWidth = 1,
            .backgroundColor = Gfx::RED600,
        },
        .pressStyle = {
            .borderRadius = RADIUS,
            .borderWidth = 1,
            .borderColor = Gfx::RED600,
            .backgroundColor = Gfx::RED700,
        },
    };

    Func<void(Node &)> _onPress;
    ButtonStyle _buttonStyle = DEFAULT;
    ButtonState _state = IDLE;

    Button(Func<void(Node &)> onPress, ButtonStyle style, Child child)
        : _Box<Button>(child),
          _onPress(std::move(onPress)),
          _buttonStyle(style) {}

    void reconcile(Button &o) override {
        _buttonStyle = o._buttonStyle;
        _onPress = std::move(o._onPress);
        _Box<Button>::reconcile(o);
    }

    BoxStyle &boxStyle() override {
        if (_state == IDLE) {
            return _buttonStyle.idleStyle;
        } else if (_state == OVER) {
            return _buttonStyle.hoverStyle;
        } else {
            return _buttonStyle.pressStyle;
        }
    }

    void event(Events::Event &e) override {
        ButtonState state = _state;

        e.handle<Events::MouseEvent>([&](auto &m) {
            if (!bound().contains(m.pos)) {
                state = IDLE;
                return false;
            }

            if (state != PRESS) {
                state = OVER;
            }

            if (m.type == Events::MouseEvent::PRESS) {
                state = PRESS;
                return true;
            } else if (m.type == Events::MouseEvent::RELEASE) {
                state = OVER;
                _onPress(*this);
                return true;
            }

            return false;
        });

        if (state != _state) {
            _state = state;
            shouldRepaint(*this);
        }
    };
};

static inline Child button(Func<void(Node &)> onPress, ButtonStyle style, Child child) {
    return makeStrong<Button>(std::move(onPress), style, child);
}

static inline Child button(Func<void(Node &)> onPress, ButtonStyle style, Str t) {
    return button(
        std::move(onPress),
        style,
        minSize(
            {Sizing::UNCONSTRAINED, 36},
            center(
                spacing(
                    {16, 6},
                    text(t)))));
}

static inline Child button(Func<void(Node &)> onPress, ButtonStyle style, Media::Icon i) {
    return button(
        std::move(onPress),
        style,
        minSize(
            {36, 36},
            center(
                spacing(
                    {6, 6},
                    icon(i)))));
}

static inline Child button(Func<void(Node &)> onPress, ButtonStyle style, Media::Icon i, Str t) {
    return button(
        std::move(onPress),
        style,
        minSize(
            {36, 36},
            spacing(
                {12, 6, 16, 6},
                hflow(
                    8,
                    Layout::Align::CENTER,
                    icon(i),
                    text(t)))));
}

static inline Child button(Func<void(Node &)> onPress, Child child) {
    return button(std::move(onPress), Button::DEFAULT, child);
}

static inline Child button(Func<void(Node &)> onPress, Str t) {
    return button(std::move(onPress), Button::DEFAULT, t);
}

static inline Child button(Func<void(Node &)> onPress, Media::Icons i) {
    return button(std::move(onPress), Button::DEFAULT, i);
}

static inline Child button(Func<void(Node &)> onPress, Media::Icons i, Str t) {
    return button(std::move(onPress), Button::DEFAULT, i, t);
}

} // namespace Karm::Ui
