#include "node.h"

namespace Grund::Device {

Node::Node() {
    static usize id = 0;
    _id = id++;
}

Node::~Node() {
    for (auto& child : _children) {
        child->_parent = nullptr;
    }
}

Res<> Node::init() {
    for (auto& child : _children) {
        try$(child->init());
    }
    return Ok();
}

Res<> Node::event(App::Event& e) {
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

Res<> Node::bubble(App::Event& e) {
    if (_parent and not e.accepted()) {
        try$(_parent->bubble(e));
    }

    return Ok();
}

Res<> Node::attach(Rc<Node> child) {
    child->_parent = this;
    _children.pushBack(child);
    try$(child->init());
    return Ok();
}

void Node::detach(Rc<Node> child) {
    child->_parent = nullptr;
    _children.removeAll(child);
}

} // namespace Grund::Device
