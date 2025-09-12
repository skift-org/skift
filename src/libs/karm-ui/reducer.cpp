module;

#include <karm-gfx/canvas.h>

export module Karm.Ui:reducer;

import Karm.Core;
import Karm.App;
import :funcs;

namespace Karm::Ui {

// MARK: Reducer ---------------------------------------------------------------

export template <typename A>
using Task = Opt<Async::_Task<Opt<A>>>;

export template <typename S, typename A, Task<A> (*R)(S&, A) = [](S& s, A a) {
    return s.reduce(a);
}>

struct Model {
    using State = S;
    using Action = A;
    static constexpr auto reduce = R;

    template <typename X, typename... Args>
    static Func<void(Node&)> bind(Args... args) {
        return bindBubble<Action>(X{std::forward<Args>(args)...});
    }

    template <typename X>
    static Func<void(Node&)> bind(X value) {
        return bindBubble<Action>(value);
    }

    template <typename X, typename... Args>
    static Opt<Func<void(Node&)>> bindIf(bool cond, Args... args) {
        if (not cond)
            return NONE;
        return bindBubble<Action>(X{std::forward<Args>(args)...});
    }

    template <typename X>
    static Opt<Func<void(Node&)>> bindIf(bool cond, X value) {
        if (not cond)
            return NONE;
        return bindBubble<Action>(value);
    }

    template <typename X>
    static void bubble(Node& n, X value = {}) {
        Ui::bubble<Action>(n, Action{std::move(value)});
    }

    template <typename X>
    static void event(Node& n, X value = {}) {
        Ui::event<Action>(n, Action{std::move(value)});
    }

    template <typename X>
    static auto map() {
        return []<typename... Args>(Node& n, Args... value) {
            Ui::bubble<Action>(n, X{std::forward<Args>(value)...});
        };
    }
};

template <typename Model>
struct Reducer :
    LeafNode<Reducer<Model>> {

    using State = typename Model::State;
    using Action = typename Model::Action;

    State _state;
    Func<Child(State const&)> _build;
    bool _rebuild = true;
    Opt<Child> _child;

    Reducer(State state, Func<Child(State const&)> build)
        : _state(std::move(state)), _build(std::move(build)) {}

    ~Reducer() {
        if (_child)
            (*_child)->detach(this);
    }

    // MARK: Build -------------------------------------------------------------

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

    // MARK: Node --------------------------------------------------------------

    void reconcile(Reducer& o) override {
        _build = std::move(o._build);
        _rebuild = true;
    }

    void paint(Gfx::Canvas& g, Math::Recti r) override {
        if (_rebuild)
            panic("paint() called on React node before build");
        (*_child)->paint(g, r);
    }

    void event(App::Event& e) override {
        if (auto a = e.is<Action>()) {
            auto task = Model::reduce(_state, *a);
            if (task != NONE) {
                Async::detach(task.take(), [this](Opt<Action> const& a) {
                    if (a)
                        Ui::bubble<Action>(*this, *a);
                });
            }
            e.accept();
            _rebuild = true;
            shouldLayout(*this);
        } else {
            ensureBuild();
            (*_child)->event(e);
        }
    }

    void bubble(App::Event& e) override {
        if (auto a = e.is<Action>()) {
            auto task = Model::reduce(_state, *a);
            if (task != NONE) {
                Async::detach(task.take(), [this](Opt<Action> const& a) {
                    if (a)
                        Ui::bubble<Action>(*this, *a);
                });
            }
            e.accept();

            _rebuild = true;
            shouldLayout(*this);
        } else {
            LeafNode<Reducer<Model>>::bubble(e);
        }
    }

    void layout(Math::Recti r) override {
        ensureBuild();
        (*_child)->layout(r);
    }

    Math::Vec2i size(Math::Vec2i s, Hint hint) override {
        ensureBuild();
        return (*_child)->size(s, hint);
    }

    Math::Recti bound() override {
        ensureBuild();
        return (*_child)->bound();
    }
};

export template <typename Model>
Child reducer(
    typename Model::State init,
    Func<Child(typename Model::State const&)> build
) {

    return makeRc<Reducer<Model>>(std::move(init), std::move(build));
}

export template <typename Model>
Child reducer(Func<Child(typename Model::State const&)> build) {
    return makeRc<Reducer<Model>>(typename Model::State{}, std::move(build));
}

// MARK: State -----------------------------------------------------------------

export template <typename T>
Child state(T init, auto build) {
    auto reduce = [](T& s, T v) -> Task<T> {
        s = v;
        return NONE;
    };

    using M = Model<T, T, reduce>;

    return reducer<M>(init, [build = std::move(build)](T const& state) {
        return build(state, [](T t) {
            return bindBubble<T>(t);
        });
    });
}

export template <typename T>
Child state(auto build) {
    return state<T>({}, std::move(build));
}

export template <typename T>
using Action = SharedFunc<void(Ui::Node&, T const&)>;

} // namespace Karm::Ui
