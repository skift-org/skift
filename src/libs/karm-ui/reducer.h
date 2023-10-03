#pragma once

#include "funcs.h"

namespace Karm::Ui {

/* --- Reducer -------------------------------------------------------------- */

template <typename S, typename A, void (*R)(S &, A)>
struct Model {
    using State = S;
    using Action = A;
    static constexpr auto reduce = R;

    template <typename X, typename... Args>
    static Func<void(Node &)> bind(Args... args) {
        return bindBubble<Action>(X{std::forward<Args>(args)...});
    }

    template <typename X>
    static Func<void(Node &)> bind(X value) {
        return bindBubble<Action>(value);
    }

    template <typename X, typename... Args>
    static Opt<Func<void(Node &)>> bindIf(bool cond, Args... args) {
        if (not cond)
            return NONE;
        return bindBubble<Action>(X{std::forward<Args>(args)...});
    }

    template <typename X>
    static Opt<Func<void(Node &)>> bindIf(bool cond, X value) {
        if (not cond)
            return NONE;
        return bindBubble<Action>(value);
    }

    template <typename X>
    static void dispatch(Node &n, X value) {
        bubble<Action>(n, Action{std::move(value)});
    }
};

template <typename Model>
struct Reducer :
    public LeafNode<Reducer<Model>> {

    using State = typename Model::State;
    using Action = typename Model::Action;

    State _state;
    Func<Child(State const &)> _build;
    bool _rebuild = true;
    Opt<Child> _child;

    Reducer(State state, Func<Child(State const &)> build)
        : _state(state), _build(std::move(build)) {}

    ~Reducer() {
        if (_child)
            (*_child)->detach(this);
    }

    /* --- Build ------------------------------------------------------------ */

    void rebuild() {
        if (_child) {
            auto tmp = (*_child)->reconcile(_build(_state));
            if (tmp) {
                (*_child)->detach(this);
                _child = tmp;
                (*_child)->attach(this);
            }
        } else {
            _child = _build(_state);
            (*_child)->attach(this);
        }
    }

    void ensureBuild() {
        if (_rebuild) {
            rebuild();
            _rebuild = false;
        }
    }

    /* --- Node ------------------------------------------------------------- */

    void reconcile(Reducer &o) override {
        _build = std::move(o._build);
        _rebuild = true;
    }

    void paint(Gfx::Context &g, Math::Recti r) override {
        if (_rebuild)
            panic("paint() called on React node before build");
        (*_child)->paint(g, r);
    }

    void event(Async::Event &e) override {
        ensureBuild();
        (*_child)->event(e);
    }

    void bubble(Async::Event &e) override {
        if (auto *a = e.is<Action>()) {
            Model::reduce(_state, *a);
            e.accept();

            _rebuild = true;
            shouldLayout(*this);
        }

        LeafNode<Reducer<Model>>::bubble(e);
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

template <typename Model>
inline Child reducer(
    typename Model::State init,
    Func<Child(typename Model::State const &)> build) {

    return makeStrong<Reducer<Model>>(init, std::move(build));
}

template <typename Model>
inline Child reducer(Func<Child(typename Model::State const &)> build) {
    return makeStrong<Reducer<Model>>(typename Model::State{}, std::move(build));
}

/* --- State ---------------------------------------------------------------- */

template <typename T>
inline Child state(T init, auto build) {
    auto reduce = [](T &s, T v) {
        s = v;
    };

    using M = Model<T, T, reduce>;

    return reducer<M>(init, [build](T const &state) {
        return build(state, [](T t) {
            return bindBubble<T>(t);
        });
    });
}

template <typename T>
inline Child state(auto build) {
    return state<T>({}, std::move(build));
}

} // namespace Karm::Ui
