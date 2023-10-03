#include "layout.h"

#include "view.h"

namespace Karm::Ui {

struct Grow;

/* --- Empty ---------------------------------------------------------------- */

struct Empty : public View<Empty> {
    Math::Vec2i _size;

    Empty(Math::Vec2i size)
        : _size(size) {}

    void reconcile(Empty &o) override {
        _size = o._size;
    }

    Math::Vec2i size(Math::Vec2i, Layout::Hint) override {
        return _size;
    }

    void paint(Gfx::Context &g, Math::Recti) override {
        if (debugShowEmptyBounds) {
            auto b = bound();
            g.debugRect(b, Gfx::WHITE.withOpacity(0.2));
            g.debugLine({b.topStart(), b.bottomEnd()}, Gfx::WHITE.withOpacity(0.2));
            g.debugLine({b.topEnd(), b.bottomStart()}, Gfx::WHITE.withOpacity(0.2));
        }
    }
};

Child empty(Math::Vec2i size) {
    return makeStrong<Empty>(size);
}

Child cond(bool cond, Child child) {
    if (cond) {
        return child;
    } else {
        return empty();
    }
}

/* --- Bound ---------------------------------------------------------------- */

struct Bound : public ProxyNode<Bound> {
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

    Math::Vec2i size(Math::Vec2i s, Layout::Hint hint) override {
        return child().size(s, hint);
    }
};

Child bound(Child child) {
    return makeStrong<Bound>(child);
}

/* --- Separator ------------------------------------------------------------ */

struct Separator : public View<Separator> {
    Math::Vec2i size(Math::Vec2i, Layout::Hint) override {
        return {1};
    }

    void paint(Gfx::Context &g, Math::Recti) override {
        g.save();
        g.fillStyle(GRAY800);
        g.fill(bound());
        g.restore();
    }
};

Child separator() {
    return makeStrong<Separator>();
}

/* --- Align ---------------------------------------------------------------- */

struct Align : public ProxyNode<Align> {
    Layout::Align _align;

    Align(Layout::Align align, Child child) : ProxyNode(child), _align(align) {}

    void layout(Math::Recti bound) override {
        auto childSize = child().size(bound.size(), _child.is<Grow>() ? Layout::Hint::MAX : Layout::Hint::MIN);
        child()
            .layout(_align.apply<isize>(
                Layout::Flow::LEFT_TO_RIGHT,
                childSize,
                bound));
    };

    Math::Vec2i size(Math::Vec2i s, Layout::Hint hint) override {
        return _align.size(child().size(s, hint), s, hint);
    }
};

Child align(Layout::Align align, Child child) {
    return makeStrong<Align>(align, child);
}

Child center(Child child) {
    return align(Layout::Align::CENTER, child);
}

Child start(Child child) {
    return align(Layout::Align::START | Layout::Align::VFILL, child);
}

Child end(Child child) {
    return align(Layout::Align::END | Layout::Align::VFILL, child);
}

Child fit(Child child) {
    return align(Layout::Align::FIT, child);
}

Child cover(Child child) {
    return align(Layout::Align::COVER, child);
}

Child hcenter(Child child) {
    return align(Layout::Align::HCENTER | Layout::Align::TOP, child);
}

Child vcenter(Child child) {
    return align(Layout::Align::VCENTER | Layout::Align::START, child);
}

Child hcenterFill(Child child) {
    return align(Layout::Align::HCENTER | Layout::Align::VFILL, child);
}

Child vcenterFill(Child child) {
    return align(Layout::Align::VCENTER | Layout::Align::HFILL, child);
}

/* --- Sizing --------------------------------------------------------------- */

struct Sizing : public ProxyNode<Sizing> {
    Math::Vec2i _min;
    Math::Vec2i _max;
    Math::Recti _rect;

    Sizing(Math::Vec2i min, Math::Vec2i max, Child child)
        : ProxyNode(child), _min(min), _max(max) {}

    Math::Recti bound() override {
        return _rect;
    }

    void layout(Math::Recti bound) override {
        _rect = bound;
        child().layout(bound);
    }

    Math::Vec2i size(Math::Vec2i s, Layout::Hint hint) override {
        auto result = child().size(s, hint);

        if (_min.x != UNCONSTRAINED) {
            result.x = max(result.x, _min.x);
        }

        if (_min.y != UNCONSTRAINED) {
            result.y = max(result.y, _min.y);
        }

        if (_max.x != UNCONSTRAINED) {
            result.x = min(result.x, _max.x);
        }

        if (_max.y != UNCONSTRAINED) {
            result.y = min(result.y, _max.y);
        }

        return result;
    }
};

Child sizing(Math::Vec2i min, Math::Vec2i max, Child child) {
    return makeStrong<Sizing>(min, max, child);
}

Child minSize(Math::Vec2i size, Child child) {
    return makeStrong<Sizing>(size, UNCONSTRAINED, child);
}

Child minSize(isize size, Child child) {
    return minSize(Math::Vec2i{size}, child);
}

Child maxSize(Math::Vec2i size, Child child) {
    return makeStrong<Sizing>(UNCONSTRAINED, size, child);
}

Child maxSize(isize size, Child child) {
    return maxSize(Math::Vec2i{size}, child);
}

Child pinSize(Math::Vec2i size, Child child) {
    return makeStrong<Sizing>(size, size, child);
}

Child pinSize(isize size, Child child) {
    return minSize(Math::Vec2i{size}, child);
}

/* --- Spacing -------------------------------------------------------------- */

struct Spacing : public ProxyNode<Spacing> {
    Layout::Spacingi _spacing;

    Spacing(Layout::Spacingi spacing, Child child)
        : ProxyNode(child), _spacing(spacing) {}

    void reconcile(Spacing &o) override {
        _spacing = o._spacing;
        ProxyNode<Spacing>::reconcile(o);
    }

    void paint(Gfx::Context &g, Math::Recti r) override {
        child().paint(g, r);
        if (debugShowLayoutBounds) {
            g.debugRect(child().bound(), Gfx::LIME);
        }
    }

    void layout(Math::Recti rect) override {
        child().layout(_spacing.shrink(Layout::Flow::LEFT_TO_RIGHT, rect));
    }

    Math::Vec2i size(Math::Vec2i s, Layout::Hint hint) override {
        return child().size(s - _spacing.all(), hint) + _spacing.all();
    }

    Math::Recti bound() override {
        return _spacing.grow(Layout::Flow::LEFT_TO_RIGHT, child().bound());
    }
};

Child spacing(Layout::Spacingi s, Child child) {
    return makeStrong<Spacing>(s, child);
}

/* --- Aspect Ratio --------------------------------------------------------- */

struct AspectRatio : public ProxyNode<AspectRatio> {
    f64 _ratio;

    AspectRatio(f64 ratio, Child child)
        : ProxyNode(child), _ratio(ratio) {}

    void reconcile(AspectRatio &o) override {
        _ratio = o._ratio;
        ProxyNode<AspectRatio>::reconcile(o);
    }

    void paint(Gfx::Context &g, Math::Recti r) override {
        child().paint(g, r);
        if (debugShowLayoutBounds) {
            g.debugRect(child().bound(), Gfx::INDIGO);
        }
    }

    void layout(Math::Recti rect) override {
        child().layout(rect);
    }

    Math::Vec2i size(Math::Vec2i s, Layout::Hint) override {
        if (s.x < s.y)
            return {s.x, (isize)(s.x * _ratio)};

        return {(isize)(s.y * _ratio), s.y};
    }

    Math::Recti bound() override {
        return child().bound();
    }
};

Child aspectRatio(f64 ratio, Child child) {
    return makeStrong<AspectRatio>(ratio, child);
}

/* --- Stack ---------------------------------------------------------------- */

struct StackLayout : public GroupNode<StackLayout> {
    using GroupNode::GroupNode;

    void layout(Math::Recti r) override {
        _bound = r;
        for (auto &child : children()) {
            child->layout(r);
        }
    }

    void event(Async::Event &e) override {
        if (e.accepted())
            return;

        for (auto &child : mutIterRev(children())) {
            child->event(e);
            if (e.accepted())
                return;
        }
    }
};

Child stack(Children children) {
    return makeStrong<StackLayout>(children);
}

/* --- Dock ----------------------------------------------------------------- */

struct DockItem : public ProxyNode<DockItem> {
    Layout::Dock _dock;

    DockItem(Layout::Dock dock, Child child)
        : ProxyNode(child), _dock(dock) {}

    Layout::Dock dock() const { return _dock; }
};

Child docked(Layout::Dock dock, Child child) {
    return makeStrong<DockItem>(dock, child);
}

Child dockTop(Child child) {
    return docked(Layout::Dock::TOP, child);
}

Child dockBottom(Child child) {
    return docked(Layout::Dock::BOTTOM, child);
}

Child dockStart(Child child) {
    return docked(Layout::Dock::START, child);
}

Child dockEnd(Child child) {
    return docked(Layout::Dock::END, child);
}

struct DockLayout : public GroupNode<DockLayout> {
    using GroupNode::GroupNode;

    static auto getDock(auto &child) -> Layout::Dock {
        if (child.template is<DockItem>()) {
            return child.template unwrap<DockItem>().dock();
        }

        return Layout::Dock::NONE;
    };

    void layout(Math::Recti bound) override {
        _bound = bound;
        auto outer = bound;

        for (auto &child : children()) {
            Math::Recti inner = child->size(outer.size(), Layout::Hint::MIN);
            child->layout(getDock(child).apply(inner, outer));
        }
    }

    static Math::Vec2i apply(Layout::Orien o, Math::Vec2i current, Math::Vec2i inner) {
        switch (o) {
        case Layout::Orien::NONE:
            current = current.max(inner);

        case Layout::Orien::HORIZONTAL:
            current.x += inner.x;
            current.y = max(current.y, inner.y);
            break;

        case Layout::Orien::VERTICAL:
            current.x = max(current.x, inner.x);
            current.y += inner.y;
            break;
        default:
            break;
        }

        return current;
    }

    Math::Vec2i size(Math::Vec2i s, Layout::Hint hint) override {
        Math::Vec2i currentSize{};
        for (auto &child : mutIterRev(children())) {
            currentSize = apply(getDock(child).orien(), child->size(currentSize, Layout::Hint::MIN), currentSize);
        }

        if (hint == Layout::Hint::MAX) {
            currentSize = currentSize.max(s);
        }

        return currentSize;
    }
};

Child dock(Children children) {
    return makeStrong<DockLayout>(children);
}

/* --- Flow ----------------------------------------------------------------- */

struct Grow : public ProxyNode<Grow> {
    isize _grow;

    Grow(Child child)
        : ProxyNode(child), _grow(1) {}

    Grow(isize grow, Child child)
        : ProxyNode(child), _grow(grow) {}

    isize grow() const {
        return _grow;
    }
};

Child grow(Opt<Child> child) {
    return makeStrong<Grow>(tryOrElse(
        child,
        []() {
            return empty();
        }));
}

Child grow(isize grow, Opt<Child> child) {
    return makeStrong<Grow>(
        grow,
        tryOrElse(
            child,
            []() {
                return empty();
            }));
}

struct FlowLayout : public GroupNode<FlowLayout> {
    using GroupNode::GroupNode;

    FlowStyle _style;

    FlowLayout(FlowStyle style, Children children)
        : GroupNode(children), _style(style) {}

    void reconcile(FlowLayout &o) override {
        _style = o._style;
        GroupNode::reconcile(o);
    }

    f64 computeGrowUnit(Math::Recti r) {
        f64 total = 0;
        f64 grows = 0;

        for (auto &child : children()) {
            if (child.is<Grow>()) {
                grows += child.unwrap<Grow>().grow();
            } else {
                total += _style.flow.getX(child->size(r.size(), Layout::Hint::MIN));
            }
        }

        f64 all = _style.flow.getWidth(r) - _style.gaps * (max(1uz, children().len()) - 1);
        f64 growTotal = max(0, all - total);
        return (growTotal) / max(1, grows);
    }

    void layout(Math::Recti r) override {
        _bound = r;

        f64 growUnit = computeGrowUnit(r);
        f64 start = _style.flow.getStart(r);

        for (auto &child : children()) {
            Math::Recti inner = {};
            auto childSize = child->size(r.size(), Layout::Hint::MIN);

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

    Math::Vec2i size(Math::Vec2i s, Layout::Hint hint) override {
        isize w{};
        isize h{hint == Layout::Hint::MAX ? _style.flow.getY(s) : 0};
        bool grow = false;

        for (auto &child : children()) {
            if (child.is<Grow>())
                grow = true;

            auto childSize = child->size(s, Layout::Hint::MIN);
            w += _style.flow.getX(childSize);
            h = max(h, _style.flow.getY(childSize));
        }

        w += _style.gaps * (max(1uz, children().len()) - 1);
        if (grow and hint == Layout::Hint::MAX) {
            w = max(_style.flow.getX(s), w);
        }

        return _style.flow.orien() == Layout::Orien::HORIZONTAL
                   ? Math::Vec2i{w, h}
                   : Math::Vec2i{h, w};
    }
};

Child flow(FlowStyle style, Children children) {
    return makeStrong<FlowLayout>(style, children);
}

/* --- Grid ----------------------------------------------------------------- */

struct Cell : public ProxyNode<Cell> {
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

Child cell(Math::Vec2i pos, Child child) {
    return makeStrong<Cell>(pos, pos, child);
}

Child cell(Math::Vec2i start, Math::Vec2i end, Child child) {
    return makeStrong<Cell>(start, end, child);
}

struct GridLayout : public GroupNode<GridLayout> {
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

        for (auto &row : _style.rows) {
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

        for (auto &column : _style.columns) {
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
        for (auto &r : _style.rows) {
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
        for (auto &c : _style.columns) {
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
        for (auto &child : children()) {
            if (child.is<Cell>()) {
                auto &cell = child.unwrap<Cell>();
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

    Math::Vec2i size(Math::Vec2i s, Layout::Hint hint) override {
        isize row = 0;
        bool rowGrow = false;
        isize growUnitRows = computeGrowUnitRows(Math::Recti{0, s});
        for (auto &r : _style.rows) {
            if (r.unit == GridUnit::GROW) {
                row += growUnitRows * r.value;
                rowGrow = true;
            } else {
                row += r.value;
            }
        }

        row += computeGapsRows();

        if (rowGrow and hint == Layout::Hint::MAX) {
            row = max(_style.flow.getY(s), row);
        }

        isize column = 0;
        bool columnGrow = false;
        isize growUnitColumns = computeGrowUnitColumns(Math::Recti{0, s});
        for (auto &c : _style.columns) {
            if (c.unit == GridUnit::GROW) {
                column += growUnitColumns * c.value;
                columnGrow = true;
            } else {
                column += c.value;
            }
        }

        column += computeGapsColumns();

        if (columnGrow and hint == Layout::Hint::MAX) {
            column = max(_style.flow.getX(s), column);
        }

        return Math::Vec2i{column, row};
    }
};

Child grid(GridStyle style, Children children) {
    return makeStrong<GridLayout>(style, children);
}

} // namespace Karm::Ui
