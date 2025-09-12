export module Vaev.Engine:values.writing;

namespace Vaev {

// https://drafts.csswg.org/css-writing-modes-4/#inline-axis
// https://drafts.csswg.org/css-writing-modes-4/#block-axis

export struct Axis {
    enum struct _Type {
        HORIZONTAL,
        VERTICAL,

        CROSS = VERTICAL,
        MAIN = HORIZONTAL,

        BLOCK = VERTICAL,
        INLINE = HORIZONTAL,
    };

    using enum _Type;

    _Type value;

    constexpr Axis(_Type value) : value(value) {
    }

    constexpr Axis cross() const {
        return value == HORIZONTAL ? VERTICAL : HORIZONTAL;
    }

    bool operator==(_Type const& value) const {
        return this->value == value;
    }

    bool operator==(Axis const& other) const = default;

    int index() const {
        return value == HORIZONTAL ? 0 : 1;
    }
};

// https://www.w3.org/TR/css-writing-modes-3/#propdef-writing-mode
export enum struct WritingMode {
    HORIZONTAL_TB,
    VERTICAL_RL,
    VERTICAL_LR,
};

// https://www.w3.org/TR/css-writing-modes-3/#propdef-direction
export enum struct Direction {
    LTR,
    RTL,
};

} // namespace Vaev
