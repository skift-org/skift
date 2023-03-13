#pragma once

#include <karm-layout/align.h>
#include <karm-layout/dock.h>
#include <karm-layout/spacing.h>

#include "node.h"

namespace Karm::Ui {

inline constexpr auto UNCONSTRAINED = -1;

/* --- Empty ---------------------------------------------------------------- */

Child empty(Math::Vec2i size = {});

Child cond(bool cond, Child child);

inline auto cond(bool c) {
    return [c](Child child) {
        return cond(c, child);
    };
}

/* --- Bound ---------------------------------------------------------------- */

Child bound(Child child);

inline auto bound() {
    return [](Child child) {
        return bound(child);
    };
}

/* --- Separator ------------------------------------------------------------ */

Child separator();

/* --- Grow ----------------------------------------------------------------- */

Child grow(Opt<Child> child);

inline auto grow() {
    return [](Child child) {
        return grow(child);
    };
}

Child grow(isize grow, Opt<Child> child);

inline auto grow(isize g) {
    return [g](Child child) {
        return grow(g, child);
    };
}

/* --- Align ---------------------------------------------------------------- */

Child align(Layout::Align align, Child child);

inline auto align(Layout::Align a) {
    return [a](Child child) {
        return align(a, child);
    };
}

Child center(Child child);

inline auto center() {
    return [](Child child) {
        return center(child);
    };
}

Child start(Child child);

inline auto start() {
    return [](Child child) {
        return start(child);
    };
}

Child end(Child child);

inline auto end() {
    return [](Child child) {
        return end(child);
    };
}

Child fit(Child child);

inline auto fit() {
    return [](Child child) {
        return fit(child);
    };
}

Child cover(Child child);

inline auto cover() {
    return [](Child child) {
        return cover(child);
    };
}

Child hcenter(Child child);

inline auto hcenter() {
    return [](Child child) {
        return hcenter(child);
    };
}

Child vcenter(Child child);

inline auto vcenter() {
    return [](Child child) {
        return vcenter(child);
    };
}

Child hcenterFill(Child child);

inline auto hcenterFill() {
    return [](Child child) {
        return hcenterFill(child);
    };
}

Child vcenterFill(Child child);

inline auto vcenterFill() {
    return [](Child child) {
        return vcenterFill(child);
    };
}

/* --- Sizing --------------------------------------------------------------- */

Child sizing(Math::Vec2i min, Math::Vec2i max, Child child);

inline auto sizing(Math::Vec2i min, Math::Vec2i max) {
    return [min, max](Child child) {
        return sizing(min, max, child);
    };
}

Child minSize(Math::Vec2i size, Child child);

inline auto minSize(Math::Vec2i size) {
    return [size](Child child) {
        return minSize(size, child);
    };
}

Child minSize(isize size, Child child);

inline auto minSize(isize size) {
    return [size](Child child) {
        return minSize(size, child);
    };
}

Child maxSize(Math::Vec2i size, Child child);

inline auto maxSize(Math::Vec2i size) {
    return [size](Child child) {
        return maxSize(size, child);
    };
}

Child maxSize(isize size, Child child);

inline auto maxSize(isize size) {
    return [size](Child child) {
        return maxSize(size, child);
    };
}

Child pinSize(Math::Vec2i size, Child child);

inline auto pinSize(Math::Vec2i size) {
    return [size](Child child) {
        return pinSize(size, child);
    };
}

Child pinSize(isize size, Child child);

inline auto pinSize(isize size) {
    return [size](Child child) {
        return pinSize(size, child);
    };
}

/* --- Spacing -------------------------------------------------------------- */

Child spacing(Layout::Spacingi s, Child child);

inline auto spacing(Layout::Spacingi s) {
    return [s](Child child) {
        return spacing(s, child);
    };
}

/* --- Aspect Ratio --------------------------------------------------------- */

Child aspectRatio(f64 ratio, Child child);

inline auto aspectRatio(f64 ratio) {
    return [ratio](Child child) {
        return aspectRatio(ratio, child);
    };
}

/* --- Stack ---------------------------------------------------------------- */

Child stack(Children children);

inline Child stack(auto... children) {
    return stack(Children{children...});
}

/* --- Dock ----------------------------------------------------------------- */

Child docked(Layout::Dock dock, Child child);

inline auto docked(Layout::Dock d) {
    return [d](Child child) {
        return docked(d, child);
    };
}

Child dockTop(Child child);

inline auto dockTop() {
    return [](Child child) {
        return dockTop(child);
    };
}

Child dockBottom(Child child);

inline auto dockBottom() {
    return [](Child child) {
        return dockBottom(child);
    };
}

Child dockStart(Child child);

inline auto dockStart() {
    return [](Child child) {
        return dockStart(child);
    };
}

Child dockEnd(Child child);

inline auto dockEnd() {
    return [](Child child) {
        return dockEnd(child);
    };
}

Child dock(Children children);

inline Child dock(Meta::Same<Child> auto... children) {
    return dock({children...});
}

/* --- Flow ----------------------------------------------------------------- */

struct FlowStyle {
    Layout::Flow flow = Layout::Flow::LEFT_TO_RIGHT;
    Layout::Align align = Layout::Align::FILL;
    isize gaps{};

    static FlowStyle horizontal(isize gaps = 0, Layout::Align align = Layout::Align::FILL) {
        return FlowStyle{Layout::Flow::LEFT_TO_RIGHT, align, gaps};
    }

    static FlowStyle vertical(isize gaps = 0, Layout::Align align = Layout::Align::FILL) {
        return FlowStyle{Layout::Flow::TOP_TO_BOTTOM, align, gaps};
    }
};

Child flow(FlowStyle style, Children children);

inline Child hflow(Meta::Same<Child> auto... children) {
    return flow({.flow = Layout::Flow::LEFT_TO_RIGHT}, {children...});
}

inline Child hflow(isize gaps, Meta::Same<Child> auto... children) {
    return flow({.flow = Layout::Flow::LEFT_TO_RIGHT, .gaps = gaps}, {children...});
}

inline Child hflow(isize gaps, Layout::Align align, Meta::Same<Child> auto... children) {
    return flow({.flow = Layout::Flow::LEFT_TO_RIGHT, .align = align, .gaps = gaps}, {children...});
}

inline Child hflow(Children children) {
    return flow({.flow = Layout::Flow::LEFT_TO_RIGHT}, children);
}

inline Child hflow(isize gaps, Children children) {
    return flow({.flow = Layout::Flow::LEFT_TO_RIGHT, .gaps = gaps}, children);
}

inline Child hflow(isize gaps, Layout::Align align, Children children) {
    return flow({.flow = Layout::Flow::LEFT_TO_RIGHT, .align = align, .gaps = gaps}, children);
}

inline Child vflow(Meta::Same<Child> auto... children) {
    return flow({.flow = Layout::Flow::TOP_TO_BOTTOM}, {children...});
}

inline Child vflow(isize gaps, Meta::Same<Child> auto... children) {
    return flow({.flow = Layout::Flow::TOP_TO_BOTTOM, .gaps = gaps}, {children...});
}

inline Child vflow(isize gaps, Layout::Align align, Meta::Same<Child> auto... children) {
    return flow({.flow = Layout::Flow::TOP_TO_BOTTOM, .align = align, .gaps = gaps}, {children...});
}

inline Child vflow(Children children) {
    return flow({.flow = Layout::Flow::TOP_TO_BOTTOM}, children);
}

inline Child vflow(isize gaps, Children children) {
    return flow({.flow = Layout::Flow::TOP_TO_BOTTOM, .gaps = gaps}, children);
}

inline Child vflow(isize gaps, Layout::Align align, Children children) {
    return flow({.flow = Layout::Flow::TOP_TO_BOTTOM, .align = align, .gaps = gaps}, children);
}

/* --- Grid ----------------------------------------------------------------- */

struct GridUnit {
    enum _Unit {
        AUTO,
        FIXED,
        GROW,
    };

    _Unit unit;
    isize value;

    static GridUnit auto_() {
        return {AUTO, 0};
    }

    static GridUnit fixed(isize value) {
        return GridUnit{FIXED, value};
    }

    static GridUnit grow(isize value = 1) {
        return GridUnit{GROW, value};
    }

    GridUnit(_Unit unit, isize value) : unit(unit), value(value) {}

    Vec<GridUnit> repeated(usize count) {
        Vec<GridUnit> units{};
        while (count--)
            units.pushBack(*this);
        return units;
    }
};

struct GridStyle {
    Vec<GridUnit> rows;
    Vec<GridUnit> columns;

    Math::Vec2i gaps;
    Layout::Flow flow = Layout::Flow::LEFT_TO_RIGHT;
    Layout::Align align = Layout::Align::FILL;

    static GridStyle simpleGrow(isize rows, isize columns, Math::Vec2i gaps = 0) {
        return GridStyle{
            GridUnit::grow().repeated(rows),
            GridUnit::grow().repeated(columns),
            gaps,
            Layout::Flow::LEFT_TO_RIGHT,
            Layout::Align::FILL,
        };
    }

    static GridStyle simpleFixed(Cons<isize, isize> rows, Cons<isize, isize> columns, Math::Vec2i gaps = {}) {
        return GridStyle{
            GridUnit::fixed(rows.cdr).repeated(rows.car),
            GridUnit::fixed(columns.cdr).repeated(columns.car),
            gaps,
            Layout::Flow::LEFT_TO_RIGHT,
            Layout::Align::FILL,
        };
    }

    static GridStyle simpleAuto(isize rows, isize columns, Math::Vec2i gaps = 0) {
        return GridStyle{
            GridUnit::auto_().repeated(rows),
            GridUnit::auto_().repeated(columns),
            gaps,
            Layout::Flow::LEFT_TO_RIGHT,
            Layout::Align::FILL,
        };
    }
};

Child cell(Math::Vec2i pos, Child child);

inline auto cell(Math::Vec2i pos) {
    return [pos](Child child) {
        return cell(pos, child);
    };
}

Child cell(Math::Vec2i start, Math::Vec2i end, Child child);

inline auto cell(Math::Vec2i start, Math::Vec2i end) {
    return [start, end](Child child) {
        return cell(start, end, child);
    };
}

Child grid(GridStyle style, Children children);

inline Child grid(GridStyle style, Meta::Same<Child> auto... children) {
    return grid(style, Children{children...});
}

} // namespace Karm::Ui
