#pragma once

#include <karm-text/run.h>

namespace Vaev::Layout {

struct Cell {
};

struct Cluster {
};

struct Line {
};

struct Prose {
    Vec<Text::Run> _runs{};
    Vec<Cluster> _clusters{};
    Vec<Line> _lines{};
};

} // namespace Vaev::Layout
