module;

#include <karm/macros>

export module Strata.Device:node;

import Karm.Core;
import Karm.App;

using namespace Karm;

namespace Strata::Device {

export struct IrqEvent {
    usize irq;
};

export struct Node :
    Meta::Pinned {

    usize _id = 0;
    Node* _parent = nullptr;
    Vec<Rc<Node>> _children = {};

    Node() {
        static usize id = 0;
        _id = id++;
    }

    virtual ~Node() {
        for (auto& child : _children) {
            child->_parent = nullptr;
        }
    }

    virtual Res<> init() {
        for (auto& child : _children) {
            try$(child->init());
        }
        return Ok();
    }

    virtual Res<> event(App::Event& e) {
        if (e.accepted()) {
            return Ok();
        }

        for (auto& child : _children) {
            try$(child->event(e));

            if (e.accepted()) {
                return Ok();
            }
        }

        return Ok();
    }

    virtual Res<> bubble(App::Event& e) {
        if (_parent and not e.accepted()) 
            try$(_parent->bubble(e));
        return Ok();
    }

    Res<> attach(Rc<Node> child) {
        child->_parent = this;
        _children.pushBack(child);
        try$(child->init());
        return Ok();
    }

    void detach(Rc<Node> child);

    auto operator<=>(Node const& o) const {
        return _id <=> o._id;
    }

    auto operator==(Node const& o) const {
        return _id == o._id;
    }
};

void Node::detach(Rc<Node> child) {
    child->_parent = nullptr;
    _children.removeAll(child);
}

} // namespace Strata::Device
