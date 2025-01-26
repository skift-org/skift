#pragma once

#include <karm-app/event.h>
#include <karm-base/checked.h>
#include <karm-base/func.h>
#include <karm-base/hash.h>
#include <karm-gfx/canvas.h>
#include <karm-logger/logger.h>
#include <karm-sys/async.h>

#include "atoms.h"
#include "macros.h"

namespace Karm::Ui {

enum struct Hint {
    MIN,
    PREFERRED,
    MAX,
};

struct Node;

using Child = Rc<Node>;
using Children = Vec<Child>;
using Visitor = Func<void(Node&)>;

// MARK: Node ------------------------------------------------------------------

using Key = Opt<Hash>;

struct Node : public App::Dispatch {
    Key _key = NONE;
    bool _consumed = false;

    struct PaintEvent {
        Math::Recti bound;
    };

    struct LayoutEvent {
    };

    struct AnimateEvent {
        f64 dt;
    };

    Key key() const {
        return _key;
    }

    virtual Opt<Child> reconcile(Child other) { return other; }

    virtual void paint(Gfx::Canvas&, Math::Recti) {}

    virtual void layout(Math::Recti) {}

    virtual Math::Vec2i size(Math::Vec2i s, Hint) { return s; }

    virtual Math::Recti bound() { panic("bound() not implemented"); }

    virtual Node* parent() { return nullptr; }

    virtual void attach(Node*) {}

    virtual void detach(Node*) {}
};

inline auto key(Hashable auto const& key) {
    return [key](Child child) {
        child->_key = hash(key);
        return child;
    };
}

template <typename T>
concept Decorator = requires(T& t, Child& c) {
    { t(c) } -> Meta::Same<Child>;
};

always_inline Child operator|(Child child, Decorator auto decorator) {
    return decorator(child);
}

always_inline Child& operator|=(Child& child, Decorator auto decorator) {
    return child = decorator(child);
}

always_inline auto operator|(Decorator auto decorator, Decorator auto decorator2) {
    return [=](Child child) {
        return decorator2(decorator(child));
    };
}

// MARK: LeafNode --------------------------------------------------------------

template <typename Crtp>
struct LeafNode : public Node {
    Node* _parent = nullptr;

    virtual void reconcile(Crtp&) {}

    Opt<Child> reconcile(Child other) override {
        // NOTE: Nodes should never be part of a state, to
        //       ensure this we check that nodes are not
        //       reused accross rebuilds
        if (other->_consumed)
            panic("reconcile() called on consumed node, did you forget to wrap the node in a slot?");

        if (this == &other.unwrap())
            panic("reconcile() called on self, did you forget to wrap the node in a slot?");

        if (not other.is<Crtp>() or _key != other->key())
            return other;

        reconcile(other.unwrap<Crtp>());
        other->_consumed = true;

        return NONE;
    }

    void bubble(App::Event& e) override {
        if (_parent and not e.accepted())
            _parent->bubble(e);
    }

    void event(App::Event&) override {
    }

    Node* parent() override {
        return _parent;
    }

    void attach(Node* parent) override {
        _parent = parent;
    }

    void detach(Node* parent) override {
        if (_parent == parent)
            _parent = nullptr;
    }
};

// MARK: GroupNode -------------------------------------------------------------

template <typename Crtp>
struct GroupNode : public LeafNode<Crtp> {
    Children _children;
    Math::Recti _bound{};

    GroupNode() = default;

    GroupNode(Children children) : _children(children) {
        for (auto& c : _children) {
            c->attach(this);
        }
    }

    ~GroupNode() {
        for (auto& c : _children) {
            c->detach(this);
        }
    }

    Children& children() {
        return _children;
    }

    Children const& children() const {
        return _children;
    }

    void reconcile(Crtp& o) override {
        auto& us = children();
        auto& them = o.children();

        for (usize i = 0; i < them.len(); i++) {
            if (i < us.len()) {
                us.replace(i, us[i]->reconcile(them[i]).unwrapOr(us[i]));
            } else {
                us.insert(i, them[i]);
            }
            us[i]->attach(this);
        }

        us.trunc(them.len());
    }

    void paint(Gfx::Canvas& g, Math::Recti r) override {
        for (auto& child : children()) {
            if (not child->bound().colide(r))
                continue;

            child->paint(g, r);
        }
    }

    void event(App::Event& e) override {
        if (e.accepted())
            return;

        for (auto& child : children()) {
            child->event(e);
            if (e.accepted())
                return;
        }
    }

    void layout(Math::Recti r) override {
        _bound = r;

        for (auto& child : children())
            child->layout(r);
    }

    Math::Recti bound() override {
        return _bound;
    }
};

// MARK: ProxyNode -------------------------------------------------------------

template <typename Crtp>
struct ProxyNode : public LeafNode<Crtp> {
    Child _child;

    ProxyNode(Child child) : _child(child) {
        _child->attach(this);
    }

    ~ProxyNode() {
        _child->detach(this);
    }

    Node& child() {
        return *_child;
    }

    Node const& child() const {
        return *_child;
    }

    void reconcile(Crtp& o) override {
        _child = _child->reconcile(o._child).unwrapOr(_child);
        _child->attach(this);
        LeafNode<Crtp>::reconcile(o);
    }

    void paint(Gfx::Canvas& g, Math::Recti r) override {
        child().paint(g, r);
    }

    void event(App::Event& e) override {
        if (e.accepted())
            return;

        child().event(e);
    }

    void layout(Math::Recti r) override {
        child().layout(r);
    }

    Math::Vec2i size(Math::Vec2i s, Hint hint) override {
        return child().size(s, hint);
    }

    Math::Recti bound() override {
        return _child->bound();
    }
};

using Slot = Func<Child()>;

using Slots = Func<Children()>;

} // namespace Karm::Ui
