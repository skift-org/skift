#pragma once

#include <karm-base/array.h>
#include <karm-sys/time.h>

namespace Karm::Cli {

static constexpr Array SPINNER = {
    "⠋",
    "⠙",
    "⠹",
    "⠸",
    "⠼",
    "⠴",
    "⠦",
    "⠧",
    "⠇",
    "⠏",
};

static inline Str spinner() {
    auto time = Sys::time().toDuration().toMSecs() / 80;
    return SPINNER[time % SPINNER.len()];
}

} // namespace Karm::Cli
