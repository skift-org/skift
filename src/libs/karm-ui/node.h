#pragma once

#include <karm-async/async.h>
#include <karm-base/checked.h>
#include <karm-base/func.h>
#include <karm-base/hash.h>
#include <karm-events/events.h>
#include <karm-gfx/context.h>
#include <karm-layout/size.h>
#include <karm-logger/logger.h>

#include "theme.h"

namespace Karm::Ui {

extern bool debugShowLayoutBounds;
extern bool debugShowRepaintBounds;
extern bool debugShowEmptyBounds;
extern bool debugShowScrollBounds;
extern bool debugShowPerfGraph;
extern int debugNodeCount;

struct Node;

using Child = Strong<Node>;
using Children = Vec<Child>;
using Visitor = Func<void(Node &)>;

/* --- Node ----------------------------------------------------------------- */

using Key = Opt<Hash>;

inline bool match(Key lhs, Key rhs) {
    if (not lhs.has() and not rhs.has())
        return true;

    if (lhs.has() and rhs.has())
        return lhs == rhs;

    return false;
}

struct Node : public Meta::Static {
    Key _key = NONE;

    struct PaintEvent {
        Math::Recti bound;
    };

    struct LayoutEvent {
    };

    struct AnimateEvent {
        f64 dt;
    };

    Node() {
        debugNodeCount++;
    }

    virtual ~Node() {
        debugNodeCount--;
    }

    Key key() const {
        return _key;
    }

    virtual Opt<Child> reconcile(Child other) { return other; }

    virtual void paint(Gfx::Context &, Math::Recti) {}

    virtual void event(Async::Event &) {}

    virtual void bubble(Async::Event &) {}

    virtual void layout(Math::Recti) {}

    virtual Math::Vec2i size(Math::Vec2i s, Layout::Hint) { return s; }

    virtual Math::Recti bound() { return {}; }

    virtual Node *parent() { return nullptr; }

    virtual void attach(Node *) {}

    virtual void detach(Node *) {}
};

inline auto key(Hashable auto const &key) {
    return [key](Child child) {
        child->_key = hash(key);
        return child;
    };
}

template <typename T>
concept Decorator = requires(T &t, Child &c) {
    { t(c) } -> Meta::Same<Child>;
};

ALWAYS_INLINE Child operator|(Child child, Decorator auto decorator) {
    return decorator(child);
}

ALWAYS_INLINE Child &operator|=(Child &child, Decorator auto decorator) {
    return child = decorator(child);
}

ALWAYS_INLINE auto operator|(Decorator auto decorator, Decorator auto decorator2) {
    return [=](Child child) {
        return decorator2(decorator(child));
    };
}

/* --- LeafNode ------------------------------------------------------------- */

template <typename Crtp>
struct LeafNode : public Node {
    Node *_parent = nullptr;

    virtual void reconcile(Crtp &) {}

    Opt<Child> reconcile(Child other) override {
        if (this == &other.unwrap()) {
            debug("reconcile() called on self");
            return NONE;
        }

        if (not other.is<Crtp>())
            return other;

        reconcile(other.unwrap<Crtp>());
        return NONE;
    }

    void bubble(Async::Event &e) override {
        if (_parent and not e.accepted())
            _parent->bubble(e);
    }

    Node *parent() override {
        return _parent;
    }

    void attach(Node *parent) override {
        _parent = parent;
    }

    void detach(Node *parent) override {
        if (_parent == parent)
            _parent = nullptr;
    }
};

/* --- GroupNode ------------------------------------------------------------ */

template <typename Crtp>
struct GroupNode : public LeafNode<Crtp> {
    Children _children;
    Math::Recti _bound{};

    GroupNode() = default;

    GroupNode(Children children) : _children(children) {
        for (auto &c : _children) {
            c->attach(this);
        }
    }

    ~GroupNode() {
        for (auto &c : _children) {
            c->detach(this);
        }
    }

    Children &children() {
        return _children;
    }

    Children const &children() const {
        return _children;
    }

    void reconcile(Crtp &o) override {
        auto &us = children();
        auto &them = o.children();

        for (usize i = 0; i < them.len(); i++) {
            if (i < us.len()) {
                us.replace(i, tryOr(us[i]->reconcile(them[i]), us[i]));
            } else {
                us.insert(i, them[i]);
            }
            us[i]->attach(this);
        }

        us.truncate(them.len());
    }

    void paint(Gfx::Context &g, Math::Recti r) override {
        for (auto &child : children()) {
            if (not child->bound().colide(r))
                continue;

            child->paint(g, r);
        }
    }

    void event(Async::Event &e) override {
        if (e.accepted())
            return;

        for (auto &child : children()) {
            child->event(e);
            if (e.accepted())
                return;
        }
    }

    void layout(Math::Recti r) override {
        _bound = r;

        for (auto &child : children()) {
            child->layout(r);
        }
    }

    Math::Recti bound() override {
        return _bound;
    }
};

/* --- ProxyNode ------------------------------------------------------------ */

template <typename Crtp>
struct ProxyNode : public LeafNode<Crtp> {
    Child _child;

    ProxyNode(Child child) : _child(child) {
        _child->attach(this);
    }

    ~ProxyNode() {
        _child->detach(this);
    }

    Node &child() {
        return *_child;
    }

    Node const &child() const {
        return *_child;
    }

    void reconcile(Crtp &o) override {
        _child = tryOr(_child->reconcile(o._child), _child);
        _child->attach(this);
        LeafNode<Crtp>::reconcile(o);
    }

    void paint(Gfx::Context &g, Math::Recti r) override {
        child().paint(g, r);
    }

    void event(Async::Event &e) override {
        if (e.accepted())
            return;

        child().event(e);
    }

    void layout(Math::Recti r) override {
        child().layout(r);
    }

    Math::Vec2i size(Math::Vec2i s, Layout::Hint hint) override {
        return child().size(s, hint);
    }

    Math::Recti bound() override {
        return _child->bound();
    }
};

} // namespace Karm::Ui
