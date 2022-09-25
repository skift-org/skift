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

/* --- Grow ----------------------------------------------------------------- */

Child grow(Child child);

Child grow(int grow, Child child);

Child grow(int g = 1);

/* --- Align ---------------------------------------------------------------- */

Child align(Layout::Align align, Child child);

Child center(Child child);

Child hcenter(Child child);

Child vcenter(Child child);

Child hcenterFill(Child child);

Child vcenterFill(Child child);

/* --- Sizing --------------------------------------------------------------- */

Child minSize(Math::Vec2i size, Child child);

Child minSize(int size, Child child);

Child maxSize(Math::Vec2i size, Child child);

Child maxSize(int size, Child child);

Child pinSize(Math::Vec2i size, Child child);

Child pinSize(int size, Child child);

/* --- Spacing -------------------------------------------------------------- */

Child spacing(Layout::Spacingi s, Child child);

/* --- Aspect Ratio --------------------------------------------------------- */

Child aspectRatio(float ratio, Child child);

/* --- Stack ---------------------------------------------------------------- */

Child stack(Children children);

inline Child stack(auto... children) {
    return stack(Children{children...});
}

/* --- Dock ----------------------------------------------------------------- */

Child docked(Layout::Dock dock, Child child);

Child dockTop(Child child);

Child dockBottom(Child child);

Child dockStart(Child child);

Child dockEnd(Child child);

Child dock(Children children);

inline Child dock(Meta::Same<Child> auto... children) {
    return dock({children...});
}

/* --- Flow ----------------------------------------------------------------- */

struct FlowStyle {
    Layout::Flow flow = Layout::Flow::LEFT_TO_RIGHT;
    Layout::Align align = Layout::Align::FILL;
    int gaps{};

    static FlowStyle horizontal(int gaps = 0, Layout::Align align = Layout::Align::FILL) {
        return FlowStyle{Layout::Flow::LEFT_TO_RIGHT, align, gaps};
    }

    static FlowStyle vertical(int gaps = 0, Layout::Align align = Layout::Align::FILL) {
        return FlowStyle{Layout::Flow::TOP_TO_BOTTOM, align, gaps};
    }
};

Child flow(FlowStyle style, Children children);

inline Child hflow(Meta::Same<Child> auto... children) {
    return flow({.flow = Layout::Flow::LEFT_TO_RIGHT}, {children...});
}

inline Child hflow(int gaps, Meta::Same<Child> auto... children) {
    return flow({.flow = Layout::Flow::LEFT_TO_RIGHT, .gaps = gaps}, {children...});
}

inline Child hflow(int gaps, Layout::Align align, Meta::Same<Child> auto... children) {
    return flow({.flow = Layout::Flow::LEFT_TO_RIGHT, .align = align, .gaps = gaps}, {children...});
}

inline Child hflow(Children children) {
    return flow({.flow = Layout::Flow::LEFT_TO_RIGHT}, children);
}

inline Child hflow(int gaps, Children children) {
    return flow({.flow = Layout::Flow::LEFT_TO_RIGHT, .gaps = gaps}, children);
}

inline Child hflow(int gaps, Layout::Align align, Children children) {
    return flow({.flow = Layout::Flow::LEFT_TO_RIGHT, .align = align, .gaps = gaps}, children);
}

inline Child vflow(Meta::Same<Child> auto... children) {
    return flow({.flow = Layout::Flow::TOP_TO_BOTTOM}, {children...});
}

inline Child vflow(int gaps, Meta::Same<Child> auto... children) {
    return flow({.flow = Layout::Flow::TOP_TO_BOTTOM, .gaps = gaps}, {children...});
}

inline Child vflow(int gaps, Layout::Align align, Meta::Same<Child> auto... children) {
    return flow({.flow = Layout::Flow::TOP_TO_BOTTOM, .align = align, .gaps = gaps}, {children...});
}

inline Child vflow(Children children) {
    return flow({.flow = Layout::Flow::TOP_TO_BOTTOM}, children);
}

inline Child vflow(int gaps, Children children) {
    return flow({.flow = Layout::Flow::TOP_TO_BOTTOM, .gaps = gaps}, children);
}

inline Child vflow(int gaps, Layout::Align align, Children children) {
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
    int value;

    static GridUnit auto_() {
        return {AUTO, 0};
    }

    static GridUnit fixed(int value) {
        return GridUnit{FIXED, value};
    }

    static GridUnit grow(int value = 1) {
        return GridUnit{GROW, value};
    }

    GridUnit(_Unit unit, int value) : unit(unit), value(value) {}

    Vec<GridUnit> repeated(size_t count) {
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

    static GridStyle simple(int rows, int columns, Math::Vec2i gaps = 0) {
        return GridStyle{
            GridUnit::grow().repeated(rows),
            GridUnit::grow().repeated(columns),
            gaps,
            Layout::Flow::LEFT_TO_RIGHT,
            Layout::Align::FILL,
        };
    }
};

Child cell(Math::Vec2i pos, Child child);

Child cell(Math::Vec2i start, Math::Vec2i end, Child child);

Child grid(GridStyle style, Children children);

inline Child grid(GridStyle style, Meta::Same<Child> auto... children) {
    return grid(style, Children{children...});
}

} // namespace Karm::Ui
