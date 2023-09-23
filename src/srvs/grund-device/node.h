#pragma once

#include <karm-base/rc.h>
#include <karm-base/vec.h>
#include <karm-events/events.h>

namespace Dev {

struct IrqEvent : public Events::BaseEvent<IrqEvent> {
    usize irq;

    IrqEvent(usize irq)
        : irq(irq) {}
};

struct Node : public Meta::Static {
    usize _id = 0;
    Node *_parent = nullptr;
    Vec<Strong<Node>> _children = {};

    Node();

    virtual ~Node();

    virtual Res<> init();

    virtual Res<> event(Events::Event &e);

    virtual Res<> bubble(Events::Event &e);

    Res<> attach(Strong<Node> child);

    void detach(Strong<Node> child);

    auto operator<=>(Node const &o) const {
        return _id <=> o._id;
    }

    auto operator==(Node const &o) const {
        return _id == o._id;
    }
};

} // namespace Dev
