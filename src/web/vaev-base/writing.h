#pragma once

namespace Vaev {

// https://drafts.csswg.org/css-writing-modes-4/#inline-axis
enum struct InlineAxis {
    HORIZONTAL,
    VERTICAL,
};

// https://drafts.csswg.org/css-writing-modes-4/#block-axis
enum struct BlockAxis {
    HORIZONTAL,
    VERTICAL,
};

// https://www.w3.org/TR/css-writing-modes-3/#propdef-writing-mode
enum struct WritingMode {
    HORIZONTAL_TB,
    VERTICAL_RL,
    VERTICAL_LR,
};

// https://www.w3.org/TR/css-writing-modes-3/#propdef-direction
enum struct Direction {
    LTR,
    RTL,
};

} // namespace Vaev
