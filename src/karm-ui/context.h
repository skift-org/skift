#pragma once

#include <embed/app.h>
#include <karm-app/client.h>
#include <karm-app/host.h>
#include <karm-base/rc.h>
#include <karm-events/events.h>
#include <karm-gfx/context.h>
#include <karm-main/base.h>
#include <karm-meta/utils.h>
#include <karm-sys/chan.h>
#include <karm-text/emit.h>

namespace Karm::Ui {

/* --- Traits --------------------------------------------------------------- */

struct Listener {
    virtual ~Listener() = default;
    virtual void dispatch(Events::Event const &) = 0;
};

/* --- Node ----------------------------------------------------------------- */

struct _Node;
struct Context;

using NodePtr = Strong<_Node>;

static int _idCounter = 0;

struct _Node : Meta::Static {
    int _id = _idCounter++;
    Vec<NodePtr> _children;
    _Node *_parent;

    Math::Vec2i _scroll;
    Math::Recti _bound;

    virtual ~_Node() = default;

    /* --- Callbacks --- */

    virtual void onMount() {
    }

    virtual void onUnmount() {}

    virtual void onEvent(Events::Event const &) {
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
        Sys::println("Inserting {} at index {}, with currently {} children", _id, index, _children.len());
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

    void dumps(Text::Emit emit) {
        emit("begin node {} {}", _id, len(_children));
        emit.newline();

        emit.indented([&] {
            for (auto &child : iter(_children)) {
                child->dumps(emit);
            }
        });

        emit("end node");
        emit.newline();
    }
};

/* --- Context -------------------------------------------------------------- */

struct Context : Meta::Static {
    struct Scope {
        NodePtr parent;
        size_t child = 0;
    };

    Vec<Scope> _stack{};

    Scope &current() {
        return last(_stack);
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

        Strong<T> child = makeStrong<T>();
        child->update(std::move(props));
        if (_stack.len() > 0) {
            parent().mount(child);
            current().child++;
        }
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
        return ctx.end();
    };
}

template <typename T>
auto Element(Decorator auto... inner) {
    return [=](Context &ctx) mutable {
        ctx.begin<T>(typename T::Props{});
        (inner(ctx), ...);
        return ctx.end();
    };
}

#define ElementBuilder$(T)                                                            \
    static inline auto T(Decorator auto... inner) { return Element<_##T>(inner...); } \
    static inline auto T(typename _##T::Props props, Decorator auto... inner) { return Element<_##T>(props, inner...); }

/* --- Widgets ---------------------------------------------------------------- */

struct _View : public _Element<None> {
};

ElementBuilder$(View);

struct ButtonProps {
};

struct _Button : public _Element<ButtonProps> {
};

ElementBuilder$(Button);

struct _Text : public _Element<String> {
};

ElementBuilder$(Text);

/* --- Layouts -------------------------------------------------------------- */

struct FlexProps {
};

struct _Flex : public _Element<FlexProps> {
};

ElementBuilder$(Flex);

struct StackProps {
};

struct _Stack : public _Element<StackProps> {
};

ElementBuilder$(Stack);

/* --- State ---------------------------------------------------------------- */

struct StateProps {
};

struct _State : public _Element<StateProps> {
};

ElementBuilder$(State);

/* --- App Render ----------------------------------------------------------- */

struct UiClient : public Karm::App::Client {
    NodePtr _root;

    UiClient(NodePtr root) : _root(root) {}

    void paint(Gfx::Context &gfx) override {
        gfx.clear(Gfx::ZINC900);

        gfx.fillStyle(Gfx::WHITE);
        gfx.fill(Math::Recti{10, 10, 64, 64});

        gfx.fill(Math::Ellipsei{64, 64, 32});

        gfx.strokeStyle(
            Gfx::stroke(Gfx::BLUE300)
                .position(Gfx::INSIDE)
                .thickness(16));

        gfx.stroke(Math::Ellipsei{64, 64, 32});

        gfx.strokeStyle(
            Gfx::stroke(Gfx::PINK300)
                .position(Gfx::OUTSIDE)
                .thickness(16));

        gfx.stroke(Math::Ellipsei{64, 64, 32});

        gfx.strokeStyle(
            Gfx::stroke(Gfx::GREEN300)
                .position(Gfx::CENTER)
                .thickness(16));

        gfx.stroke(Math::Ellipsei{64, 64, 32});
    }

    void handle(Events::Event &) override {}
};

ExitCode render(auto tree) {
    Context ctx{};
    NodePtr root = tree(ctx);
    Text::Emit emit{Sys::out()};
    root->dumps(emit);

    Box<Karm::App::Client> client = makeBox<UiClient>(root);
    auto host = try$(Embed::makeHost(std::move(client)));
    return host->run();
}

} // namespace Karm::Ui
