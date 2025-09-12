module;

#include <karm-gfx/canvas.h>
#include <karm-math/align.h>
#include <karm-math/insets.h>

export module Karm.Ui:layout;

import Karm.App;
import :node;
import :view;
import :atoms;

namespace Karm::Ui {

export constexpr auto UNCONSTRAINED = -1;

// MARK: Empty -----------------------------------------------------------------

struct Empty : View<Empty> {
    Math::Vec2i _size;

    Empty(Math::Vec2i size)
        : _size(size) {}

    void reconcile(Empty& o) override {
        _size = o._size;
    }

    Math::Vec2i size(Math::Vec2i, Hint) override {
        return _size;
    }

    void paint(Gfx::Canvas&, Math::Recti) override {}
};

export Child empty(Math::Vec2i size = {}) {
    return makeRc<Empty>(size);
}

export Child cond(bool cond, Child child) {
    if (cond)
        return child;
    return empty();
}

export auto cond(bool c) {
    return [c](Child child) {
        return cond(c, child);
    };
}

// MARK: Bound -----------------------------------------------------------------

struct Bound : ProxyNode<Bound> {
    Math::Recti _bound;

    Bound(Child child)
        : ProxyNode(child) {}

    Math::Recti bound() override {
        return _bound;
    }

    void layout(Math::Recti bound) override {
        _bound = bound;
        child().layout(bound);
    }

    Math::Vec2i size(Math::Vec2i s, Hint hint) override {
        return child().size(s, hint);
    }
};

export Child bound(Child child) {
    return makeRc<Bound>(child);
}

export auto bound() {
    return [](Child child) {
        return bound(child);
    };
}

struct Placed : ProxyNode<Placed> {
    Math::Recti _bound;
    Math::Recti _place;

    Placed(Math::Recti place, Child child)
        : ProxyNode(child), _place(place) {}

    void reconcile(Placed& o) override {
        _place = o._place;
        ProxyNode<Placed>::reconcile(o);
    }

    Math::Recti bound() override {
        return _bound;
    }

    void layout(Math::Recti bound) override {
        _bound = bound;
        auto place = _place;
        place.xy = place.xy + _bound.xy;
        child().layout(place);
    }

    Math::Vec2i size(Math::Vec2i s, Hint) override {
        return s;
    }
};

export Child placed(Math::Recti place, Child child) {
    return makeRc<Placed>(place, child);
}

export auto placed(Math::Recti bound) {
    return [bound](Child child) {
        return placed(bound, child);
    };
}

// MARK: Grow ------------------------------------------------------------------

struct Grow : ProxyNode<Grow> {
    isize _grow;

    Grow(Child child)
        : ProxyNode(child), _grow(1) {}

    Grow(isize grow, Child child)
        : ProxyNode(child), _grow(grow) {}

    isize grow() const {
        return _grow;
    }
};

export Child grow(Opt<Child> child) {
    return makeRc<Grow>(
        child.unwrapOrElse([] {
            return empty();
        })
    );
}

export auto grow() {
    return [](Child child) {
        return grow(child);
    };
}

export Child grow(isize grow, Opt<Child> child) {
    return makeRc<Grow>(
        grow,
        child.unwrapOrElse([] {
            return empty();
        })
    );
}

export auto grow(isize g) {
    return [g](Child child) {
        return grow(g, child);
    };
}

// MARK: Align -----------------------------------------------------------------

struct Align : ProxyNode<Align> {
    Math::Align _align;

    Align(Math::Align align, Child child) : ProxyNode(child), _align(align) {}

    void layout(Math::Recti bound) override {
        auto childSize = child().size(
            bound.size(), _child.is<Grow>()
                              ? Hint::MAX
                              : Hint::MIN
        );

        child()
            .layout(_align.apply<isize>(
                Math::Flow::LEFT_TO_RIGHT,
                childSize,
                bound
            ));
    };

    Math::Vec2i size(Math::Vec2i s, Hint hint) override {
        if (hint == Hint::MAX)
            return _align.maxSize(child().size(s, hint), s);
        return _align.minSize(child().size(s, hint));
    }
};

export Child align(Math::Align align, Child child) {
    return makeRc<Align>(align, child);
}

export auto align(Math::Align a) {
    return [a](Child child) {
        return align(a, child);
    };
}

export Child center(Child child) {
    return align(Math::Align::CENTER, child);
}

export auto center() {
    return [](Child child) {
        return center(child);
    };
}

export Child start(Child child) {
    return align(Math::Align::START | Math::Align::VFILL, child);
}

export auto start() {
    return [](Child child) {
        return start(child);
    };
}

export Child end(Child child) {
    return align(Math::Align::END | Math::Align::VFILL, child);
}

export auto end() {
    return [](Child child) {
        return end(child);
    };
}

export Child fit(Child child) {
    return align(Math::Align::FIT, child);
}

export auto fit() {
    return [](Child child) {
        return fit(child);
    };
}

export Child cover(Child child) {
    return align(Math::Align::COVER, child);
}

export auto cover() {
    return [](Child child) {
        return cover(child);
    };
}

export Child hcenter(Child child) {
    return align(Math::Align::HCENTER | Math::Align::TOP, child);
}

export auto hcenter() {
    return [](Child child) {
        return hcenter(child);
    };
}

export Child vcenter(Child child) {
    return align(Math::Align::VCENTER | Math::Align::START, child);
}

export auto vcenter() {
    return [](Child child) {
        return vcenter(child);
    };
}

export Child hcenterFill(Child child) {
    return align(Math::Align::HCENTER | Math::Align::VFILL, child);
}

export auto hcenterFill() {
    return [](Child child) {
        return hcenterFill(child);
    };
}

export Child vcenterFill(Child child) {
    return align(Math::Align::VCENTER | Math::Align::HFILL, child);
}

export auto vcenterFill() {
    return [](Child child) {
        return vcenterFill(child);
    };
}

// MARK: Sizing ----------------------------------------------------------------

struct Sizing : ProxyNode<Sizing> {
    Math::Vec2i _min;
    Math::Vec2i _max;
    Math::Recti _rect;

    Sizing(Math::Vec2i min, Math::Vec2i max, Child child)
        : ProxyNode(child), _min(min), _max(max) {}

    Math::Recti bound() override {
        return _rect;
    }

    void reconcile(Sizing& o) override {
        _min = o._min;
        _max = o._max;
        ProxyNode<Sizing>::reconcile(o);
    }

    void layout(Math::Recti bound) override {
        _rect = bound;
        child().layout(bound);
    }

    Math::Vec2i size(Math::Vec2i s, Hint hint) override {
        if (_max.x != UNCONSTRAINED) {
            s.x = min(s.x, _max.x);
        }

        if (_max.y != UNCONSTRAINED) {
            s.y = min(s.y, _max.y);
        }

        auto result = child().size(s, hint);

        if (_min.x != UNCONSTRAINED) {
            result.x = max(result.x, _min.x);
        }

        if (_min.y != UNCONSTRAINED) {
            result.y = max(result.y, _min.y);
        }

        return result;
    }
};

export Child sizing(Math::Vec2i min, Math::Vec2i max, Child child) {
    return makeRc<Sizing>(min, max, child);
}

export auto sizing(Math::Vec2i min, Math::Vec2i max) {
    return [min, max](Child child) {
        return sizing(min, max, child);
    };
}

export Child minSize(Math::Vec2i size, Child child) {
    return makeRc<Sizing>(size, UNCONSTRAINED, child);
}

export auto minSize(Math::Vec2i size) {
    return [size](Child child) {
        return minSize(size, child);
    };
}

export Child minSize(isize size, Child child) {
    return minSize(Math::Vec2i{size}, child);
}

export auto minSize(isize size) {
    return [size](Child child) {
        return minSize(size, child);
    };
}

export Child maxSize(Math::Vec2i size, Child child) {
    return makeRc<Sizing>(UNCONSTRAINED, size, child);
}

export auto maxSize(Math::Vec2i size) {
    return [size](Child child) {
        return maxSize(size, child);
    };
}

export Child maxSize(isize size, Child child) {
    return maxSize(Math::Vec2i{size}, child);
}

export auto maxSize(isize size) {
    return [size](Child child) {
        return maxSize(size, child);
    };
}

export Child pinSize(Math::Vec2i size, Child child) {
    return makeRc<Sizing>(size, size, child);
}

export auto pinSize(Math::Vec2i size) {
    return [size](Child child) {
        return pinSize(size, child);
    };
}

export Child pinSize(isize size, Child child) {
    return pinSize(Math::Vec2i{size}, child);
}

export auto pinSize(isize size) {
    return [size](Child child) {
        return pinSize(size, child);
    };
}

// MARK: Insets ---------------------------------------------------------------

struct Insets : ProxyNode<Insets> {
    Math::Insetsi _insets;

    Insets(Math::Insetsi insets, Child child)
        : ProxyNode(child), _insets(insets) {}

    void reconcile(Insets& o) override {
        _insets = o._insets;
        ProxyNode<Insets>::reconcile(o);
    }

    void paint(Gfx::Canvas& g, Math::Recti r) override {
        child().paint(g, r);
    }

    void layout(Math::Recti rect) override {
        child().layout(rect.shrink(_insets));
    }

    Math::Vec2i size(Math::Vec2i s, Hint hint) override {
        return child().size(s - _insets.all(), hint) + _insets.all();
    }

    Math::Recti bound() override {
        return child().bound().grow(_insets);
    }
};

export Child insets(Math::Insetsi s, Child child) {
    return makeRc<Insets>(s, child);
}

export auto insets(Math::Insetsi s) {
    return [s](Child child) {
        return insets(s, child);
    };
}

// MARK: Aspect Ratio ----------------------------------------------------------

struct AspectRatio : ProxyNode<AspectRatio> {
    f64 _ratio;

    AspectRatio(f64 ratio, Child child)
        : ProxyNode(child), _ratio(ratio) {}

    void reconcile(AspectRatio& o) override {
        _ratio = o._ratio;
        ProxyNode<AspectRatio>::reconcile(o);
    }

    void paint(Gfx::Canvas& g, Math::Recti r) override {
        child().paint(g, r);
    }

    Math::Vec2i size(Math::Vec2i s, Hint) override {
        if (s.x < s.y)
            return {s.x, (isize)(s.x * _ratio)};

        return {(isize)(s.y * _ratio), s.y};
    }

    Math::Recti bound() override {
        return child().bound();
    }
};

export Child aspectRatio(f64 ratio, Child child) {
    return makeRc<AspectRatio>(ratio, child);
}

export auto aspectRatio(f64 ratio) {
    return [ratio](Child child) {
        return aspectRatio(ratio, child);
    };
}

// MARK: Stack -----------------------------------------------------------------

struct StackLayout : GroupNode<StackLayout> {
    using GroupNode::GroupNode;

    void event(App::Event& e) override {
        if (e.accepted())
            return;

        for (auto& child : mutIterRev(children())) {
            child->event(e);
            if (e.accepted())
                return;
        }
    }

    Math::Vec2i size(Math::Vec2i s, Hint hint) override {
        isize w{};
        isize h{};

        for (auto& child : children()) {
            auto childSize = child->size(s, hint);
            w = max(w, childSize.x);
            h = max(h, childSize.y);
        }

        return {w, h};
    }
};

export Child stack(Children children) {
    return makeRc<StackLayout>(children);
}

export Child stack(auto... children) {
    return stack(Children{children...});
}

// MARK: Flow ------------------------------------------------------------------

struct FlowStyle {
    Math::Flow flow = Math::Flow::LEFT_TO_RIGHT;
    Math::Align align = Math::Align::FILL;
    isize gaps{};

    static FlowStyle horizontal(isize gaps = 0, Math::Align align = Math::Align::FILL) {
        return FlowStyle{Math::Flow::LEFT_TO_RIGHT, align, gaps};
    }

    static FlowStyle vertical(isize gaps = 0, Math::Align align = Math::Align::FILL) {
        return FlowStyle{Math::Flow::TOP_TO_BOTTOM, align, gaps};
    }
};

struct FlowLayout : GroupNode<FlowLayout> {
    using GroupNode::GroupNode;

    FlowStyle _style;

    FlowLayout(FlowStyle style, Children children)
        : GroupNode(children), _style(style) {}

    void reconcile(FlowLayout& o) override {
        _style = o._style;
        GroupNode::reconcile(o);
    }

    f64 _computeGrowUnit(Math::Recti r) {
        f64 total = 0;
        f64 grows = 0;

        for (auto& child : children()) {
            if (child.is<Grow>()) {
                grows += child.unwrap<Grow>().grow();
            } else {
                total += _style.flow.getX(child->size(r.size(), Hint::MIN));
            }
        }

        f64 all = _style.flow.getWidth(r) - _style.gaps * (max(1uz, children().len()) - 1);
        f64 growTotal = max(0, all - total);
        return (growTotal) / max(1, grows);
    }

    void layout(Math::Recti r) override {
        _bound = r;

        f64 growUnit = _computeGrowUnit(r);
        f64 start = _style.flow.getStart(r);

        for (auto& child : children()) {
            Math::Recti inner = {};
            auto childSize = child->size(r.size(), Hint::MIN);

            inner = _style.flow.setStart(inner, (isize)start);
            if (child.is<Grow>()) {
                inner = _style.flow.setWidth(inner, (isize)(growUnit * child.unwrap<Grow>().grow()));
            } else {
                inner = _style.flow.setWidth(inner, _style.flow.getX(childSize));
            }

            inner = _style.flow.setTop(inner, _style.flow.getTop(r));
            inner = _style.flow.setBottom(inner, _style.flow.getBottom(r));

            child->layout(_style.align.apply(_style.flow, Math::Recti{childSize}, inner));
            start += _style.flow.getWidth(inner) + _style.gaps;
        }
    }

    Math::Vec2i size(Math::Vec2i s, Hint hint) override {
        isize w{};
        isize h{hint == Hint::MAX ? _style.flow.getY(s) : 0};
        bool grow = false;

        for (auto& child : children()) {
            if (child.is<Grow>())
                grow = true;

            auto childSize = child->size(s, Hint::MIN);
            w += _style.flow.getX(childSize);
            h = max(h, _style.flow.getY(childSize));
        }

        w += _style.gaps * (max(1uz, children().len()) - 1);
        if (grow and hint == Hint::MAX) {
            w = max(_style.flow.getX(s), w);
        }

        return _style.flow.orien() == Math::Orien::HORIZONTAL
                   ? Math::Vec2i{w, h}
                   : Math::Vec2i{h, w};
    }
};

export Child flow(FlowStyle style, Children children) {
    return makeRc<FlowLayout>(style, children);
}

export Child hflow(Meta::Convertible<Child> auto... children) {
    return flow({.flow = Math::Flow::LEFT_TO_RIGHT}, {children...});
}

export Child hflow(isize gaps, Meta::Convertible<Child> auto... children) {
    return flow({.flow = Math::Flow::LEFT_TO_RIGHT, .gaps = gaps}, {children...});
}

export Child hflow(isize gaps, Math::Align align, Meta::Convertible<Child> auto... children) {
    return flow({.flow = Math::Flow::LEFT_TO_RIGHT, .align = align, .gaps = gaps}, {children...});
}

export Child hflow(Children children) {
    return flow({.flow = Math::Flow::LEFT_TO_RIGHT}, children);
}

export Child hflow(isize gaps, Children children) {
    return flow({.flow = Math::Flow::LEFT_TO_RIGHT, .gaps = gaps}, children);
}

export Child hflow(isize gaps, Math::Align align, Children children) {
    return flow({.flow = Math::Flow::LEFT_TO_RIGHT, .align = align, .gaps = gaps}, children);
}

export Child vflow(Meta::Convertible<Child> auto... children) {
    return flow({.flow = Math::Flow::TOP_TO_BOTTOM}, {children...});
}

export Child vflow(isize gaps, Meta::Convertible<Child> auto... children) {
    return flow({.flow = Math::Flow::TOP_TO_BOTTOM, .gaps = gaps}, {children...});
}

export Child vflow(isize gaps, Math::Align align, Meta::Convertible<Child> auto... children) {
    return flow({.flow = Math::Flow::TOP_TO_BOTTOM, .align = align, .gaps = gaps}, {children...});
}

export Child vflow(Children children) {
    return flow({.flow = Math::Flow::TOP_TO_BOTTOM}, children);
}

export Child vflow(isize gaps, Children children) {
    return flow({.flow = Math::Flow::TOP_TO_BOTTOM, .gaps = gaps}, children);
}

export Child vflow(isize gaps, Math::Align align, Children children) {
    return flow({.flow = Math::Flow::TOP_TO_BOTTOM, .align = align, .gaps = gaps}, children);
}

// MARK: Grid ------------------------------------------------------------------

export struct GridUnit {
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
        return {FIXED, value};
    }

    static GridUnit grow(isize value = 1) {
        return {GROW, value};
    }

    GridUnit(_Unit unit, isize value) : unit(unit), value(value) {}

    Vec<GridUnit> repeated(usize count) {
        Vec<GridUnit> units{};
        while (count--)
            units.pushBack(*this);
        return units;
    }
};

export struct GridStyle {
    Vec<GridUnit> rows;
    Vec<GridUnit> columns;

    Math::Vec2i gaps;
    Math::Flow flow = Math::Flow::LEFT_TO_RIGHT;
    Math::Align align = Math::Align::FILL;

    static GridStyle simpleGrow(isize rows, isize columns, Math::Vec2i gaps = 0) {
        return GridStyle{
            GridUnit::grow().repeated(rows),
            GridUnit::grow().repeated(columns),
            gaps,
            Math::Flow::LEFT_TO_RIGHT,
            Math::Align::FILL,
        };
    }

    static GridStyle simpleFixed(Pair<isize, isize> rows, Pair<isize, isize> columns, Math::Vec2i gaps = {}) {
        return GridStyle{
            GridUnit::fixed(rows.v1).repeated(rows.v0),
            GridUnit::fixed(columns.v1).repeated(columns.v0),
            gaps,
            Math::Flow::LEFT_TO_RIGHT,
            Math::Align::FILL,
        };
    }

    static GridStyle simpleAuto(isize rows, isize columns, Math::Vec2i gaps = 0) {
        return GridStyle{
            GridUnit::auto_().repeated(rows),
            GridUnit::auto_().repeated(columns),
            gaps,
            Math::Flow::LEFT_TO_RIGHT,
            Math::Align::FILL,
        };
    }
};

struct Cell : ProxyNode<Cell> {
    Math::Vec2i _start{};
    Math::Vec2i _end{};

    Cell(Math::Vec2i start, Math::Vec2i end, Child child)
        : ProxyNode(child), _start(start), _end(end) {}

    Math::Vec2i start() const {
        return _start;
    }

    Math::Vec2i end() const {
        return _end;
    }
};

export Child cell(Math::Vec2i pos, Child child) {
    return makeRc<Cell>(pos, pos, child);
}

export auto cell(Math::Vec2i pos) {
    return [pos](Child child) {
        return cell(pos, child);
    };
}

export Child cell(Math::Vec2i start, Math::Vec2i end, Child child) {
    return makeRc<Cell>(start, end, child);
}

export auto cell(Math::Vec2i start, Math::Vec2i end) {
    return [start, end](Child child) {
        return cell(start, end, child);
    };
}

struct GridLayout : GroupNode<GridLayout> {
    struct _Dim {
        isize start;
        isize size;

        isize end() const {
            return start + size;
        }
    };

    GridStyle _style;
    Vec<_Dim> _rows;
    Vec<_Dim> _columns;

    GridLayout(GridStyle style, Children children)
        : GroupNode(children), _style(style) {}

    isize computeGapsRows() {
        return _style.gaps.y * (max(1uz, _style.rows.len()) - 1);
    }

    isize computeGapsColumns() {
        return _style.gaps.x * (max(1uz, _style.columns.len()) - 1);
    }

    isize computeGrowUnitRows(Math::Recti r) {
        isize total = 0;
        isize grows = 0;

        for (auto& row : _style.rows) {
            if (row.unit == GridUnit::GROW) {
                grows += row.value;
            } else {
                total += row.value;
            }
        }

        isize all = _style.flow.getHeight(r) - computeGapsRows();
        isize growTotal = max(0, all - total);

        return (growTotal) / max(1, grows);
    }

    isize computeGrowUnitColumns(Math::Recti r) {
        isize total = 0;
        isize grows = 0;

        for (auto& column : _style.columns) {
            if (column.unit == GridUnit::GROW) {
                grows += column.value;
            } else {
                total += column.value;
            }
        }

        isize all = _style.flow.getWidth(r) - computeGapsColumns();
        isize growTotal = max(0, all - total);

        return (growTotal) / max(1, grows);
    }

    void place(Child child, Math::Vec2i pos) {
        place(child, pos, pos);
    }

    void place(Child child, Math::Vec2i start, Math::Vec2i end) {
        auto startRow = _rows[start.y];
        auto startColumn = _columns[start.x];

        auto endRow = _rows[end.y];
        auto endColumn = _columns[end.x];

        auto childRect = Math::Recti{
            startColumn.start,
            startRow.start,
            endColumn.end() - startColumn.start,
            endRow.end() - startRow.start,
        };

        child->layout(childRect);
    }

    void layout(Math::Recti r) override {
        _bound = r;

        // compute the dimensions of the grid
        _rows.clear();
        isize growUnitRows = computeGrowUnitRows(r);
        isize row = _style.flow.getTop(r);
        for (auto& r : _style.rows) {
            if (r.unit == GridUnit::GROW) {
                _rows.pushBack({_Dim{row, growUnitRows * r.value}});
                row += growUnitRows * r.value;
            } else {
                _rows.pushBack({_Dim{row, r.value}});
                row += r.value;
            }

            row += _style.gaps.y;
        }

        _columns.clear();
        isize growUnitColumns = computeGrowUnitColumns(r);
        isize column = _style.flow.getStart(r);
        for (auto& c : _style.columns) {
            if (c.unit == GridUnit::GROW) {
                _columns.pushBack({_Dim{column, growUnitColumns * c.value}});
                column += growUnitColumns * c.value;
            } else {
                _columns.pushBack({_Dim{column, c.value}});
                column += c.value;
            }

            column += _style.gaps.x;
        }

        // layout the children
        isize index = 0;
        for (auto& child : children()) {
            if (child.is<Cell>()) {
                auto& cell = child.unwrap<Cell>();
                auto start = cell.start();
                auto end = cell.end();
                place(child, start, end);
                index = end.y * _columns.len() + end.x;
            } else {
                isize row = index / _columns.len();
                isize column = index % _columns.len();

                place(child, {column, row});
            }
            index++;
        }
    }

    Math::Vec2i size(Math::Vec2i s, Hint hint) override {
        isize row = 0;
        bool rowGrow = false;
        isize growUnitRows = computeGrowUnitRows(Math::Recti{0, s});
        for (auto& r : _style.rows) {
            if (r.unit == GridUnit::GROW) {
                row += growUnitRows * r.value;
                rowGrow = true;
            } else {
                row += r.value;
            }
        }

        row += computeGapsRows();

        if (rowGrow and hint == Hint::MAX) {
            row = max(_style.flow.getY(s), row);
        }

        isize column = 0;
        bool columnGrow = false;
        isize growUnitColumns = computeGrowUnitColumns(Math::Recti{0, s});
        for (auto& c : _style.columns) {
            if (c.unit == GridUnit::GROW) {
                column += growUnitColumns * c.value;
                columnGrow = true;
            } else {
                column += c.value;
            }
        }

        column += computeGapsColumns();

        if (columnGrow and hint == Hint::MAX) {
            column = max(_style.flow.getX(s), column);
        }

        return Math::Vec2i{column, row};
    }
};

export Child grid(GridStyle style, Children children) {
    return makeRc<GridLayout>(style, children);
}

export Child grid(GridStyle style, Meta::Convertible<Child> auto... children) {
    return grid(style, Children{children...});
}

} // namespace Karm::Ui
