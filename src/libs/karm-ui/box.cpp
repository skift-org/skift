#include "box.h"

namespace Karm::Ui {

struct Box : public _Box<Box> {
    using _Box<Box>::_Box;
    BoxStyle _style;

    Box(BoxStyle style, Child child)
        : _Box(child), _style(style) {}

    void reconcile(Box &o) override {
        _style = o._style;
        _Box<Box>::reconcile(o);
    }

    BoxStyle &boxStyle() override {
        return _style;
    }
};

Child box(BoxStyle style, Child inner) {
    return makeStrong<Box>(style, inner);
}

} // namespace Karm::Ui
