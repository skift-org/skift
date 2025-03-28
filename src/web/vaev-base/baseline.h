#pragma once

#include <karm-base/base.h>
#include <karm-io/emit.h>

#include "keywords.h"

namespace Vaev::Style {

// https://www.w3.org/TR/css-inline-3/#baseline-source
using BaselineSource = Union<Keywords::Auto, Keywords::First, Keywords::Last>;

// https://www.w3.org/TR/css-inline-3/#alignment-baseline-property
using AlignmentBaseline = Union<
    Keywords::Baseline,
    Keywords::TextBottom,
    Keywords::Alphabetic,
    Keywords::Ideographic,
    Keywords::Middle,
    Keywords::Central,
    Keywords::Mathematical,
    Keywords::TextTop>;

// https://www.w3.org/TR/css-inline-3/#dominant-baseline-property
using DominantBaseline = Union<
    Keywords::Auto,
    Keywords::Baseline,
    Keywords::TextBottom,
    Keywords::Alphabetic,
    Keywords::Ideographic,
    Keywords::Middle,
    Keywords::Central,
    Keywords::Mathematical,
    Keywords::TextTop>;

struct Baseline {
    BaselineSource source = Keywords::AUTO;
    AlignmentBaseline alignment = Keywords::BASELINE;
    DominantBaseline dominant = Keywords::AUTO;

    void repr(Io::Emit& e) const {
        e("(baselines");
        e(" source={}", source);
        e(" alignment={}", alignment);
        e(" dominant={}", dominant);
        e(")");
    }
};

} // namespace Vaev::Style
