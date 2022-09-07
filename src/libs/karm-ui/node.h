#pragma once

#include <karm-base/func.h>
#include <karm-base/rc.h>
#include <karm-debug/logger.h>
#include <karm-events/events.h>
#include <karm-gfx/context.h>
#include <karm-layout/size.h>

namespace Karm::Ui {

inline constexpr bool DEBUG = false;

struct Node;

using Child = Strong<Node>;
using Other = Strong<Node>;
using Children = Vec<Child>;
using Visitor = Func<void(Node &)>;

struct Node {
    virtual ~Node() = default;

    virtual Opt<Child> reconcile(Other o) { return o; }

    virtual void paint(Gfx::Context &, Math::Recti) {}

    virtual void event(Events::Event &) {}

    virtual void bubble(Events::Event &) {}

    virtual void layout(Math::Recti) {}

    virtual Math::Vec2i size(Math::Vec2i s, Layout::Hint) { return s; }

    virtual Math::Recti bound() { return {}; }

    virtual void visit(Visitor &) {}

    virtual Node *parent() { return nullptr; }

    virtual void attach(Node *) {}

    virtual void detach(Node *) {}

    virtual void *query(Meta::Id) { return nullptr; }
};

template <typename Crtp>
struct Widget : public Node {
    Node *_parent = nullptr;

    virtual void reconcile(Crtp &) {}

    Opt<Child> reconcile(Other o) override {
        if (!o.is<Crtp>()) {
            return o;
        }
        reconcile(o.unwrap<Crtp>());
        return NONE;
    }

    void bubble(Events::Event &e) override {
        if (_parent)
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

    void *query(Meta::Id id) override {
        if (id == Meta::makeId<Crtp>()) {
            return static_cast<Crtp *>(this);
        }

        return _parent ? _parent->query(id) : nullptr;
    }
};

} // namespace Karm::Ui
