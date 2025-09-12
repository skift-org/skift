module;

#include <karm-math/rect.h>

export module Karm.Kira:resizable;

import Karm.App;
import Karm.Ui;

namespace Karm::Kira {

export enum struct ResizeHandle {
    TOP,
    START,
    BOTTOM,
    END
};

struct Resizable : Ui::ProxyNode<Resizable> {
    Math::Vec2i _size;
    Opt<Ui::Send<Math::Vec2i>> _onChange;

    Resizable(Ui::Child child, Math::Vec2i size, Opt<Ui::Send<Math::Vec2i>> onChange)
        : ProxyNode<Resizable>(child),
          _size(size),
          _onChange(std::move(onChange)) {}

    void reconcile(Resizable& o) override {
        if (o._onChange) {
            _size = o._size;
        }
        _onChange = std::move(o._onChange);
        ProxyNode<Resizable>::reconcile(o);
    }

    void bubble(App::Event& e) override {
        if (auto de = e.is<Ui::DragEvent>()) {
            if (de->type == Ui::DragEvent::DRAG) {
                _size = _size + de->delta;
                auto minSize = child().size({}, Ui::Hint::MIN);
                _size = _size.max(minSize);
                if (_onChange) {
                    _onChange(*this, _size);
                } else {
                    Ui::shouldLayout(*this);
                }
                e.accept();
            }
        }

        ProxyNode<Resizable>::bubble(e);
    }

    Math::Vec2i size(Math::Vec2i s, Ui::Hint hint) override {
        return child()
            .size(s, hint)
            .max(_size);
    }
};

export Ui::Child resizable(Ui::Child child, Math::Vec2i size, Opt<Ui::Send<Math::Vec2i>> onChange) {
    return makeRc<Resizable>(child, size, std::move(onChange));
}

export auto resizable(Math::Vec2i size, Opt<Ui::Send<Math::Vec2i>> onChange) {
    return [size, onChange = std::move(onChange)](Ui::Child child) mutable -> Ui::Child {
        return resizable(child, size, std::move(onChange));
    };
}

static Ui::Child _resizeHandle(Math::Vec2i dir) {
    return Ui::empty(4) |
           Ui::box({
               .backgroundFill = Ui::GRAY800,
           }) |
           Ui::dragRegion(dir);
}

export Ui::Child resizable(Ui::Child child, ResizeHandle handlePosition, Math::Vec2i size, Opt<Ui::Send<Math::Vec2i>> onChange) {
    if (handlePosition == ResizeHandle::TOP) {
        return Ui::stack(
                   child,
                   Ui::vflow(
                       _resizeHandle({0, -1})
                   )
               ) |
               resizable(size, std::move(onChange));
    } else if (handlePosition == ResizeHandle::START) {
        return Ui::stack(
                   child,
                   Ui::hflow(
                       _resizeHandle({-1, 0})
                   )
               ) |
               resizable(size, std::move(onChange));
    } else if (handlePosition == ResizeHandle::BOTTOM) {
        return Ui::stack(
                   child,
                   Ui::vflow(
                       Ui::grow(NONE),
                       _resizeHandle({0, 1})
                   )
               ) |
               resizable(size, std::move(onChange));
    } else if (handlePosition == ResizeHandle::END) {
        return Ui::stack(
                   child,
                   Ui::hflow(
                       Ui::grow(NONE),
                       _resizeHandle({1, 0})
                   )
               ) |
               resizable(size, std::move(onChange));
    } else {
        unreachable();
    }
}

export auto resizable(ResizeHandle handlePosition, Math::Vec2i size, Opt<Ui::Send<Math::Vec2i>> onChange) {
    return [handlePosition, size, onChange](Ui::Child child) mutable -> Ui::Child {
        return resizable(child, handlePosition, size, onChange);
    };
}

} // namespace Karm::Kira
