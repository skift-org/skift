#include "resizable.h"

namespace Karm::Ui {

struct Resizable : public ProxyNode<Resizable> {
    Math::Vec2i _size;
    OnChange<Math::Vec2i> _onChange;

    Resizable(Child child, Math::Vec2i size, OnChange<Math::Vec2i> onChange)
        : ProxyNode<Resizable>(child),
          _size(size),
          _onChange(std::move(onChange)) {}

    void reconcile(Resizable &o) override {
        if (o._onChange)
            _size = o._size;
        _onChange = std::move(o._onChange);
        ProxyNode<Resizable>::reconcile(o);
    }

    void bubble(App::Event &e) override {
        if (auto de = e.is<DragEvent>()) {
            if (de->type == DragEvent::DRAG) {
                _size = _size + de->delta;
                auto minSize = child().size({}, Hint::MIN);
                _size = _size.max(minSize);
                if (_onChange) {
                    _onChange(*this, _size);
                } else {
                    shouldLayout(*this);
                }
                e.accept();
            }
        }

        ProxyNode<Resizable>::bubble(e);
    }

    Math::Vec2i size(Math::Vec2i s, Hint hint) override {
        return child()
            .size(s, hint)
            .max(_size);
    }
};

Child resizable(Child child, Math::Vec2i size, OnChange<Math::Vec2i> onChange) {
    return makeStrong<Resizable>(child, size, std::move(onChange));
}

static Child _resizeHandle(Math::Vec2i dir) {
    return empty(4) |
           box({
               .backgroundFill = GRAY800,
           }) |
           dragRegion(dir);
}

Child resizable(Child child, ResizeHandle handlePosition, Math::Vec2i size, OnChange<Math::Vec2i> onChange) {
    if (handlePosition == ResizeHandle::TOP) {
        return stack(
                   child,
                   vflow(
                       _resizeHandle({0, -1})
                   )
               ) |
               resizable(size, std::move(onChange));
    } else if (handlePosition == ResizeHandle::START) {
        return stack(
                   child,
                   hflow(
                       _resizeHandle({-1, 0})
                   )
               ) |
               resizable(size, std::move(onChange));
    } else if (handlePosition == ResizeHandle::BOTTOM) {
        return stack(
                   child,
                   hflow(
                       grow(NONE),
                       _resizeHandle({0, 1})
                   )
               ) |
               resizable(size, std::move(onChange));
    } else if (handlePosition == ResizeHandle::END) {
        return stack(
                   child,
                   vflow(
                       grow(NONE),
                       _resizeHandle({1, 0})
                   )
               ) |
               resizable(size, std::move(onChange));
    } else {
        unreachable();
    }
}

} // namespace Karm::Ui
