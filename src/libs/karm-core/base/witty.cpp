export module Karm.Core:base.witty;

import :base.string;

namespace Karm {

namespace {

Array WITTY = {
#include "defs/witty.inc"
};

Array NICE = {
#include "defs/nice.inc"
};

Array WHOLESOME = {
#include "defs/wholesome.inc"
};

Array GOOD = {
    "🤘", "👍", "👏", "👌", "🔥", "💯", "💪", "🎉", "🎊",
    "🎈", "🏆", "🏅", "👀", "🗣️", "🗣️🗣️", "🗣️🗣️🗣️",
    "🗣️🔥", "🗿", ":^)", "🐀"
};

Array BAD = {
    "❌", "💔", "😢", "😭", "😞", "😟", "😦", "😧", "😨",
    "😩", "😪", "😫", "😭", "😮", "😰", "😱", "😲", "😳",
    "😵", "😶", "😷", "🙁", "🙃", "☝️🤓", "💀", "🥀💔", "🐀"
};

} // namespace

export Str witty(usize seed) {
    if (seed == 0)
        return "Witty comment unavailable :(";
    return WITTY[seed % WITTY.len()];
}

export Str nice(usize seed) {
    if (seed == 0)
        return "Nice comment unavailable :(";
    return NICE[seed % NICE.len()];
}

export Str wholesome(usize seed) {
    if (seed == 0)
        return "Wholesome comment unavailable :(";
    return WHOLESOME[seed % WHOLESOME.len()];
}

export Str goodEmoji(usize seed) {
    if (seed == 0)
        return "Good comment unavailable :(";
    return GOOD[seed % GOOD.len()];
}

export Str badEmoji(usize seed) {
    if (seed == 0)
        return "Bad comment unavailable :(";
    return BAD[seed % BAD.len()];
}

} // namespace Karm
