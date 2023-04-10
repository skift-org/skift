#pragma once

#include <karm-sys/time.h>

#include "funcs.h"

namespace Karm::Ui {

/* --- React ---------------------------------------------------------------- */

template <typename Crtp>
struct React : public LeafNode<Crtp> {
    bool _rebuild = true;
    Opt<Child> _child;

    ~React() {
        if (_child)
            (*_child)->detach(this);
    }

    virtual Child build() = 0;

    void rebuild() {
        auto start = Sys::now();

        auto newChild = build();

        if (_child) {
            auto tmp = (*_child)->reconcile(newChild);
            if (tmp) {
                (*_child)->detach(this);
                _child = tmp;
                (*_child)->attach(this);
            }
        } else {
            _child = newChild;
            (*_child)->attach(this);
        }

        auto elapsed = Sys::now() - start;
        if (elapsed.toMSecs() > 1)
            logWarn("Rebuild took {}ms", elapsed.toMSecs());
    }

    void ensureBuild() {
        if (_rebuild) {
            rebuild();
            _rebuild = false;
        }
    }

    void reconcile(Crtp &o) override {
        if (_child) {
            if (o._child) {
                _child = (*_child)->reconcile(*o._child);
                (*_child)->attach(this);
            }
        } else {
            if (o._child) {
                _child = o._child;
                (*_child)->attach(this);
            }
        }
        LeafNode<Crtp>::reconcile(o);
    }

    void paint(Gfx::Context &g, Math::Recti r) override {
        if (_rebuild)
            panic("paint() called on React node before build");
        (*_child)->paint(g, r);
    }

    void event(Events::Event &e) override {
        if (e.accepted)
            return;

        ensureBuild();
        (*_child)->event(e);
    }

    void bubble(Events::Event &e) override {
        if (e.is<Events::BuildEvent>()) {
            _rebuild = true;
            Ui::shouldLayout(*this);
        } else
            LeafNode<Crtp>::bubble(e);
    }

    void layout(Math::Recti r) override {
        ensureBuild();
        (*_child)->layout(r);
    }

    Math::Vec2i size(Math::Vec2i s, Layout::Hint hint) override {
        ensureBuild();
        return (*_child)->size(s, hint);
    }

    Math::Recti bound() override {
        ensureBuild();
        return (*_child)->bound();
    }
};

/* --- Lazy ----------------------------------------------------------------- */

struct Lazy : public React<Lazy> {
    Func<Child()> _build;
    Lazy(Func<Child()> build)
        : _build(std::move(build)) {}

    Child build() override {
        return _build();
    }
};

inline Child lazy(Func<Child()> build) {
    return makeStrong<Lazy>(std::move(build));
}

/* --- State ---------------------------------------------------------------- */

template <typename T>
struct _State : public React<_State<T>> {
    struct Wrap {
        _State<T> &_s;

        Wrap(_State<T> &s) : _s(s) {}

        void update(T t) {
            _s._value = t;
            shouldRebuild(_s);
        }

        T const &value() const {
            return _s._value;
        }

        Func<void(Node &)> bind(auto f) {
            return [*this, f](Node &) mutable {
                update(f(value()));
            };
        }

        Func<void(Node &)> bindValue(T v) {
            return [*this, v](Node &) mutable {
                update(v);
            };
        }

        Func<void(Node &)> bindToggle() {
            return [*this](Node &) mutable {
                update(not value());
            };
        }

        void dispatch(auto f) { update(f(value())); }
    };

    T _value;
    Func<Child(Wrap)> _build;

    _State(T initial, Func<Child(Wrap)> build)
        : _value(initial), _build(std::move(build)) {}

    void reconcile(_State &o) override {
        _build = std::move(o._build);
        React<_State>::reconcile(o);
        shouldRebuild(*this);
    }

    Child build() override {
        return _build(Wrap(*this));
    }
};

template <typename T>
using State = typename _State<T>::Wrap;

template <typename T>
Child state(T initial, Func<Child(State<T>)> build) {
    return makeStrong<_State<T>>(initial, std::move(build));
}

/* --- Reducer -------------------------------------------------------------- */

template <typename Action>
struct ActionDispatchEvent :
    public Events::BaseEvent<ActionDispatchEvent<Action>> {
    Action action;

    ActionDispatchEvent(Action action)
        : action(std::move(action)) {}
};

template <typename Action>
inline void dispatchAction(Node &n, Action action) {
    ActionDispatchEvent<Action> e{std::move(action)};
    n.bubble(e);
}

template <typename Action>
inline Func<void(Node &)> bindAction(Action action) {
    return [action](Node &n) {
        dispatchAction(n, action);
    };
}

template <typename D, typename A, D (*R)(D, A), void (*I)(Node &, Events::Event &) = nullptr>
struct Model {
    using Data = D;
    using Action = A;
    using Reduce = decltype(R);
    using Intent = decltype(I);
    static constexpr auto reduce = R;
    static constexpr auto intent = I;

    template <typename X, typename... Args>
    static void dispatch(Node &n, Args... args) {
        dispatchAction<Action>(n, X{std::forward<Args>(args)...});
    }

    template <typename X, typename... Args>
    static Func<void(Node &)> bind(Args... args) {
        return bindAction<Action>(X{std::forward<Args>(args)...});
    }

    template <typename X>
    static Func<void(Node &)> bind(X value) {
        return bindAction<Action>(value);
    }

    template <typename X, typename... Args>
    static Opt<Func<void(Node &)>> bindIf(bool cond, Args... args) {
        if (cond) {
            return bindAction<Action>(X{std::forward<Args>(args)...});
        }

        return NONE;
    }

    template <typename X>
    static Opt<Func<void(Node &)>> bindIf(bool cond, X value) {
        if (cond) {
            return bindAction<Action>(value);
        }
        return NONE;
    }
};

template <typename Model>
struct Reducer :
    public React<Reducer<Model>> {

    using Data = typename Model::Data;
    using Action = typename Model::Action;

    Data _data;
    Func<Child(Data const &)> _build;

    Reducer(Data data, Func<Child(Data const &)> build)
        : _data(data), _build(std::move(build)) {}

    void bubble(Events::Event &e) override {
        if (e.is<ActionDispatchEvent<Action>>()) {
            auto action = e.unwrap<ActionDispatchEvent<Action>>().action;
            _data = Model::reduce(_data, action);
            shouldRebuild(*this);
        } else {
            React<Reducer>::bubble(e);
        }
    }

    void event(Events::Event &e) override {
        if constexpr (Model::intent) {
            Model::intent(*this, e);
        }
        React<Reducer>::event(e);
    }

    Child build() override {
        return _build(_data);
    }
};

template <typename Model>
inline Child reducer(
    typename Model::Data initial,
    Func<Child(typename Model::Data const &)> build) {

    return makeStrong<Reducer<Model>>(initial, std::move(build));
}

template <typename Model>
inline Child reducer(Func<Child(typename Model::Data const &)> build) {

    return makeStrong<Reducer<Model>>(typename Model::Data{}, std::move(build));
}

} // namespace Karm::Ui
