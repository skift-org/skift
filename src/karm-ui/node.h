#pragma once

#include <karm-app/client.h>
#include <karm-app/host.h>
#include <karm-base/func.h>
#include <karm-base/rc.h>
#include <karm-debug/logger.h>
#include <karm-events/events.h>
#include <karm-gfx/context.h>

#include "dock.h"
#include "spacing.h"

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

    virtual void visit(Visitor) {}

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

template <typename Crtp>
struct Proxy : public Node<Crtp> {
    Child _child;

    Proxy(Child child) : _child(child) {}

    _Node &child() {
        return *_child;
    }

    _Node const &child() const {
        return *_child;
    }

    void reconcile(Crtp &o) override {
        _child = tryOr(_child->reconcile(o._child), _child);
    }

    void paint(Gfx::Context &g) const override {
        child().paint(g);
    }

    void event(Events::Event &e) override {
        child().event(e);
    }

    void layout(Math::Recti r) override {
        child().layout(r);
    }

    Math::Vec2i size(Math::Vec2i s) const override {
        return child().size(s);
    }

    Math::Recti bound() const override {
        return _child->bound();
    }

    void visit(Visitor v) override {
        v(*_child);
    }
};

template <typename Crtp>
struct Parent : public Node<Crtp> {
    Children _children;
    Math::Recti _bound;

    Parent() = default;

    Parent(Children children) : _children(children) {}

    Children &children() {
        return _children;
    }

    Children const &children() const {
        return _children;
    }

    void reconcile(Crtp &o) override {
        auto &us = children();
        auto &them = o.children();

        for (size_t i = 0; i < them.len(); i++) {
            if (i < us.len()) {
                us.insert(i, tryOr(us[i]->reconcile(them[i]), us[i]));
            } else {
                us.insert(i, them[i]);
            }
        }

        us.truncate(them.len());
    }

    void paint(Gfx::Context &g) const override {
        for (auto &child : children()) {
            child->paint(g);
        }
    }

    void event(Events::Event &e) override {
        for (auto &child : children()) {
            child->event(e);
            if (e.accepted)
                return;
        }
    }

    void layout(Math::Recti r) override {
        _bound = r;

        for (auto &child : children()) {
            child->layout(r);
        }
    }

    Math::Recti bound() const override {
        return _bound;
    }

    Math::Vec2i size(Math::Vec2i s) const override {
        for (auto &child : children()) {
            s = child->size(s);
        }
        return s;
    }

    void visit(Visitor v) override {
        for (auto &child : children()) {
            v(*child);
        }
    }
};

/* --- Layouts -------------------------------------------------------------- */

struct FlowLayout : public Parent<FlowLayout> {
    using Parent::Parent;

    void layout(Math::Recti r) override {
        for (auto &child : children()) {
            child->layout(r);
        }
    }
};

Child flow(Children children) {
    return makeStrong<FlowLayout>(children);
}

Child flow(auto... children) {
    return flow(Children{children...});
}

struct GridLayout : public Parent<GridLayout> {
    using Parent::Parent;
};

Child grid(Children children) {
    return makeStrong<GridLayout>(children);
}

Child grid(auto... children) {
    return grid(Children{children...});
}

struct DockItem : public Proxy<DockItem> {
    Dock _dock;

    DockItem(Dock dock, Child child) : Proxy(child), _dock(dock) {}

    Dock dock() const { return _dock; }
};

template <typename... T>
Child docked(T &&...args) {
    return makeStrong<DockItem>(std::forward<T>(args)...);
}

struct DockLayout : public Parent<DockLayout> {
    using Parent::Parent;

    void layout(Math::Recti bound) override {
        _bound = bound;
        auto outer = bound;

        auto getDock = [&](auto &child) -> Dock {
            if (child.template is<DockItem>()) {
                return child.template unwrap<DockItem>().dock();
            }

            return Dock::NONE;
        };

        for (auto &child : children()) {
            Math::Recti inner = child->size(outer.size());
            child->layout(getDock(child).apply(inner, outer));
        }
    }
};

Child dock(Children children) {
    return makeStrong<DockLayout>(children);
}

template <typename... Args>
Child dock(Args... args) {
    return dock({std::forward<Args>(args)...});
}

struct StackLayout : public Parent<StackLayout> {
    using Parent::Parent;

    void layout(Math::Recti r) override {
        for (auto &child : children()) {
            child->layout(r);
        }
    }
};

Child stack(Children children) {
    return makeStrong<StackLayout>(children);
}

Child stack(auto... children) {
    return stack(Children{children...});
}

/* --- Reactive ------------------------------------------------------------- */

template <typename Crtp>
struct Reactive : public Node<Crtp> {
};

template <typename T>
struct State : public Reactive<State<T>> {
};

struct Timer : public Reactive<Timer> {
};

/* --- Proxies -------------------------------------------------------------- */

struct Box : public Proxy<Box> {
    Gfx::Color _color;
    Spacingi _spacing;

    Box(Spacingi spacing, Child child)
        : Proxy(child), _spacing(spacing) {}

    Box(Gfx::Color color, Child child)
        : Proxy(child), _color(color), _spacing{} {}

    Box(Gfx::Color color, Spacingi spacing, Child child)
        : Proxy(child), _color(color), _spacing(spacing) {}

    void reconcile(Box &o) override {
        _color = o._color;
        _spacing = o._spacing;

        Proxy<Box>::reconcile(o);
    }

    void paint(Gfx::Context &g) const override {
        g.save();

        g.fillStyle(_color);
        g.fill(bound());
        g.origin(_spacing.topStart());
        child().paint(g);
        g.restore();
    }

    void layout(Math::Recti rect) override {
        child().layout(_spacing.shrink(Flow::LEFT_TO_RIGHT, rect));
    }

    Math::Vec2i size(Math::Vec2i s) const override {
        return child().size(s - _spacing.all()) + _spacing.all();
    }

    Math::Recti bound() const override {
        return _spacing.grow(Flow::LEFT_TO_RIGHT, child().bound());
    }
};

template <typename... T>
Child box(T &&...args) {
    return makeStrong<Box>(std::forward<T>(args)...);
}

struct Button : public Proxy<Button> {
};

template <typename... Args>
Child button(Args &&...args) {
    return makeStrong<Button>(std::forward<Args>(args)...);
}

/* --- Views ---------------------------------------------------------------- */

template <typename Crtp>
struct View : public Node<Crtp> {
    Math::Recti _bound;

    Math::Recti bound() const override {
        return _bound;
    }

    void layout(Math::Recti bound) override {
        _bound = bound;
    }
};

struct Empty : public View<Empty> {
    Math::Vec2i _size;

    Empty(Math::Vec2i size = {}) : _size(size) {}

    void reconcile(Empty &o) override {
        _size = o._size;
    }

    Math::Vec2i size(Math::Vec2i) const override {
        return _size;
    }
};

template <typename... Args>
Child empty(Args &&...args) {
    return makeStrong<Empty>(std::forward<Args>(args)...);
}

struct Text : public View<Text> {
    String _text;

    Text(String text) : _text(text) {}

    void reconcile(Text &o) override {
        _text = o._text;
    }

    void paint(Gfx::Context &g) const override {
        g.fill({0, 0}, _text);
    }
};

template <typename... Args>
Child text(Args &&...args) {
    return makeStrong<Text>(std::forward<Args>(args)...);
}

struct Image : public View<Image> {
};

template <typename... Args>
Child image(Args &&...args) {
    return makeStrong<Image>(std::forward<Args>(args)...);
}

struct Toggle : public View<Toggle> {
};

template <typename... Args>
Child toggle(Args &&...args) {
    return makeStrong<Toggle>(std::forward<Args>(args)...);
}

struct Input : public View<Input> {
};

template <typename... Args>
Child input(Args &&...args) {
    return makeStrong<Input>(std::forward<Args>(args)...);
}

struct Icon : public View<Icon> {
};

template <typename... Args>
Child icon(Args &&...args) {
    return makeStrong<Icon>(std::forward<Args>(args)...);
}

/* --- Host ----------------------------------------------------------------- */

struct Host : public Karm::App::Client {
    Child _root;

    Host(Child root) : _root(root) {}

    void paint(Gfx::Context &g) override {
        _root->paint(g);
    }

    void event(Events::Event &e) override {
        _root->event(e);
    }

    void layout(Math::Recti r) override {
        _root->layout(r);
    }
};

static inline ExitCode show(Child root) {
    return App::run<Host>(root);
}

} // namespace Karm::Ui
