#pragma once

#include <karm-base/rc.h>
#include <karm-gfx/context.h>
#include <karm-meta/utils.h>

#include "events.h"

namespace Karm::Ui {

/* --- Traits --------------------------------------------------------------- */

struct Listener {
    virtual ~Listener() = default;
    virtual void dispatch(Event const &) = 0;
};

/* --- Node ----------------------------------------------------------------- */

struct _Node;
struct Context;

using NodePtr = Strong<_Node>;

struct _Node : Meta::Static {
    Vec<NodePtr> _children;
    _Node *_parent;

    Math::Vec2i _orgin;
    Math::Recti _bound;

    _Node(Context &){};

    virtual ~_Node() = default;

    /* --- Callbacks --- */

    virtual void onMount() {}

    virtual void onUnmount() {}

    virtual void onEvent(Event const &) {
    }

    virtual void onLayout(Math::Recti) {
        for (auto &child : _children) {
            // by default stack childrens on top of each other
            child->layout(innerBound());
        }
    }

    virtual void onPaint(Gfx::Context &ctx) {
        for (auto &child : _children) {
            child->paint(ctx);
        }
    }

    /* --- Children --- */

    size_t childLen() const {
        return _children.len();
    }

    NodePtr childAt(size_t index) {
        return _children[index];
    }

    void mount(NodePtr child) {
        mountAt(child, childLen());
    }

    void mountAt(NodePtr child, size_t index) {
        _children.insert(index, child);
        child->_parent = this;
        child->onMount();
    }

    void unmount(NodePtr child) {
        _children.removeAll(child);
        child->_parent = nullptr;
        child->onUnmount();
    }

    void truncate(size_t len) {
        for (size_t i = len; i < _children.len(); i++) {
            _children[i]->_parent = nullptr;
            _children[i]->onUnmount();
        }

        _children.truncate(len);
    }

    /* --- Events --- */

    /* --- Layout --- */

    Math::Recti outerBound() const {
        return _bound;
    }

    Math::Recti innerBound() const {
        return {{}, _bound.wh};
    }

    void layout(Math::Recti bound) {
        _bound = bound;
        onLayout(innerBound());
    }

    /* --- Drawing --- */

    void paint(Gfx::Context &ctx) {
        onPaint(ctx);
    }

    /* --- Misc --- */

    Ordr cmp(_Node const &other) const {
        return this == &other ? Ordr::EQUAL : Ordr::LESS;
    }
};

/* --- Context -------------------------------------------------------------- */

struct App : Meta::Static {
    virtual ~App() = default;
};

Strong<App> makeApp();

struct Context : Meta::Static {
    struct Scope {
        NodePtr parent;
        size_t child = 0;
    };

    App &_app;
    Vec<Scope> _stack;

    Context(App &app) : _app(app) {}

    Scope &current() {
        return first(_stack);
    }

    _Node &parent() {
        return *current().parent;
    }

    _Node &child() {
        return *parent()._children[current().child];
    }

    template <typename T>
    NodePtr reconcile(typename T::Props &&props) {
        if (_stack.len() > 0) {
            if (current().child < parent().childLen()) {
                NodePtr child = parent().childAt(current().child);

                if (child.is<T>()) {
                    auto c = child.as<T>().take();
                    c->update(std::move(props));
                    current().child++;
                    return c;
                }
            }
        }

        Strong<T> child = makeStrong<T>(*this);
        child->update(std::move(props));
        if (_stack.len() > 0)
            parent().mount(child);
        return child;
    }

    template <typename T>
    inline void begin(typename T::Props &&props) {
        _stack.pushBack({reconcile<T>(std::move(props))});
    }

    NodePtr end() {
        parent().truncate(current().child);
        return _stack.popBack().parent;
    }
};

/* --- Element -------------------------------------------------------------- */

template <typename _Props>
struct _Element : public _Node {
    using Props = _Props;
    Props _props;

    _Element(Context &ctx) : _Node(ctx) {}

    void update(Props &&props) {
        _props = props;
    }
};

template <typename T>
concept Decorator = requires(T t, Context &ctx) {
    {t(ctx)};
};

template <typename T>
auto Element(typename T::Props props, auto... inner) {
    return [=](Context &ctx) mutable {
        ctx.begin<T>(std::move(props));
        (inner(ctx), ...);
        ctx.end();
    };
}

template <typename T>
auto Element(Decorator auto... inner) {
    return [=](Context &ctx) mutable {
        ctx.begin<T>(typename T::Props{});
        (inner(ctx), ...);
        ctx.end();
    };
}

#define ElementBuilder$(T)                                                            \
    static inline auto T(Decorator auto... inner) { return Element<_##T>(inner...); } \
    static inline auto T(typename _##T::Props props, Decorator auto... inner) { return Element<_##T>(props, inner...); }

/* --- Window ---------------------------------------------------------------- */

struct WindowProps {};

struct _Window : public _Element<WindowProps> {
    using _Element::_Element;
};

ElementBuilder$(Window);

/* --- Widgets ---------------------------------------------------------------- */

struct ButtonProps {};

struct _Button : public _Element<ButtonProps> {
    using _Element::_Element;
};

ElementBuilder$(Button);

struct _Text : public _Element<String> {
    using _Element::_Element;
};

ElementBuilder$(Text);

/* --- Layouts -------------------------------------------------------------- */

struct FlexProps {
};

struct _Flex : public _Element<FlexProps> {
    using _Element::_Element;
};

ElementBuilder$(Flex);

struct StackProps {
};

struct _Stack : public _Element<StackProps> {
    using _Element::_Element;
};

ElementBuilder$(Stack);

/* --- State ---------------------------------------------------------------- */

struct StateProps {
};

struct _State : public _Element<StateProps> {
    using _Element::_Element;
};

ElementBuilder$(State);

} // namespace Karm::Ui
