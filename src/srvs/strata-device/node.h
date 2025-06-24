#pragma once

#include <karm-base/rc.h>
#include <karm-base/res.h>
#include <karm-base/vec.h>

import Karm.App;

namespace Strata::Device {

struct IrqEvent {
    usize irq;
};

struct Node :
    Meta::Pinned {

    usize _id = 0;
    Node* _parent = nullptr;
    Vec<Rc<Node>> _children = {};

    Node();

    virtual ~Node();

    virtual Res<> init();

    virtual Res<> event(App::Event& e);

    virtual Res<> bubble(App::Event& e);

    Res<> attach(Rc<Node> child);

    void detach(Rc<Node> child);

    auto operator<=>(Node const& o) const {
        return _id <=> o._id;
    }

    auto operator==(Node const& o) const {
        return _id == o._id;
    }
};

} // namespace Strata::Device
