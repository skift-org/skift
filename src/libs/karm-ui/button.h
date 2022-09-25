#pragma once

#include "box.h"

namespace Karm::Ui {

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

    ButtonStyle withRadius(float radius) const;
};

using OnPress = Opt<Func<void(Node &)>>;

Child button(OnPress onPress, ButtonStyle style, Child child);

Child button(OnPress onPress, ButtonStyle style, Str t);

Child button(OnPress onPress, ButtonStyle style, int size, Str t);

Child button(OnPress onPress, ButtonStyle style, Media::Icon i);

Child button(OnPress onPress, ButtonStyle style, Media::Icon i, Str t);

Child button(OnPress onPress, Child child);

Child button(OnPress onPress, Str t);

Child button(OnPress onPress, int size, Str t);

Child button(OnPress onPress, Media::Icons i);

Child button(OnPress onPress, Media::Icons i, Str t);

} // namespace Karm::Ui
