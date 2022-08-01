#pragma once

#include <karm-base/func.h>
#include <karm-base/rc.h>
#include <karm-events/events.h>
#include <karm-gfx/context.h>

namespace Karm::Ui {

struct _Node;

using Child = Strong<_Node>;
using Other = Strong<_Node>;
using Children = Vec<Child>;
using Visitor = Func<void(_Node &)>;

struct _Node {
    virtual ~_Node() = default;

    virtual Opt<Child> reconcile(Other o) { return o; }

    virtual void paint(Gfx::Context &) const {}

    virtual void event(Events::Event &) {}

    virtual void layout(Math::Recti) {}

    virtual Math::Vec2i size(Math::Vec2i s) const { return s; }

    virtual Math::Recti bound() const { return {}; }

    virtual void visit(Visitor &) {}

    virtual _Node *parent() { return nullptr; }

    virtual void mount(_Node *) {}

    virtual void unmount() {}

    virtual void *query(Meta::Id) { return nullptr; }
};

template <typename Crtp>
struct Node : public _Node {
    virtual void reconcile(Crtp &) {}

    Opt<Child> reconcile(Other o) override {
        if (!o.is<Crtp>()) {
            return o;
        }
        reconcile(o.unwrap<Crtp>());
        return NONE;
    }
};

} // namespace Karm::Ui
