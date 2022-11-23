#pragma once

#include "funcs.h"

namespace Karm::Ui {

/* --- React ---------------------------------------------------------------- */

template <typename Crtp>
struct React : public LeafNode<Crtp> {
    bool _rebuild = true;
    Opt<Child> _child;

    ~React() {
        _child.with([&](auto &child) {
            child->detach(this);
        });
    }

    virtual Child build() = 0;

    void rebuild() {
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
    }

    void ensureBuild() {
        if (_rebuild) {
            rebuild();
            _rebuild = false;
        }
    }

    void reconcile(Crtp &) override {}

    void paint(Gfx::Context &g, Math::Recti r) override {
        _child.with([&](auto &child) {
            child->paint(g, r);
        });
    }

    void event(Events::Event &e) override {
        _child.with([&](auto &child) {
            child->event(e);
        });
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

        _child.with([&](auto &child) {
            child->layout(r);
        });
    }

    Math::Vec2i size(Math::Vec2i s, Layout::Hint hint) override {
        ensureBuild();

        return _child.visit([&](auto &child) {
            return child->size(s, hint);
        });
    }

    Math::Recti bound() override {
        ensureBuild();

        return _child.visit([&](auto &child) {
            return child->bound();
        });
    }

    void visit(Visitor &v) override {
        ensureBuild();
        v(**_child);
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
                update(!value());
            };
        }

        void dispatch(auto f) { update(f(value())); }
    };

    T _value;
    Func<Child(Wrap)> _build;

    _State(T initial, Func<Child(Wrap)> build)
        : _value(initial), _build(std::move(build)) {}

    void reconcile(_State &o) override {
        _value = o._value;
        _build = std::move(o._build);
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
struct ActionDispatch {
    virtual ~ActionDispatch() = default;
    virtual void dispatch(Action) = 0;
};

template <typename Action>
inline void dispatchAction(Node &n, Action action) {
    auto &d = queryParent<ActionDispatch<Action>>(n);
    d.dispatch(action);
}

template <typename Action>
inline Func<void(Node &)> bindAction(Action action) {
    return [action](Node &n) {
        dispatchAction(n, action);
    };
}

template <typename D, typename A>
struct Model {
    using Data = D;
    using Action = A;
    using Reduce = Func<Data(Data, Action)>;

    template <typename X, typename... Args>
    static void dispatch(Node &n, Args... args) {
        dispatchAction<Action>(n, {std::forward<Args>(args)...});
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
        } else {
            return NONE;
        }
    }

    template <typename X>
    static Opt<Func<void(Node &)>> bindIf(bool cond, X value) {
        if (cond) {
            return bindAction<Action>(value);
        } else {
            return NONE;
        }
    }
};

template <typename Model>
struct Reducer :
    public React<Reducer<Model>>,
    public ActionDispatch<typename Model::Action> {

    using Data = typename Model::Data;
    using Action = typename Model::Action;
    using Reduce = typename Model::Reduce;

    Data _data;
    Reduce _reducer;
    Func<Child(Data)> _build;

    Reducer(Data data, Reduce reducer, Func<Child(Data)> build)
        : _data(data), _reducer(std::move(reducer)), _build(std::move(build)) {}

    void dispatch(Action action) override {
        _data = _reducer(_data, action);
        shouldRebuild(*this);
    }

    Child build() override {
        return _build(_data);
    }

    void *query(Meta::Id id) override {
        if (id == Meta::makeId<ActionDispatch<Action>>())
            return static_cast<ActionDispatch<Action> *>(this);
        return React<Reducer<Model>>::query(id);
    }
};

template <typename Model>
inline Child reducer(
    typename Model::Data initial,
    typename Model::Reduce reducer,
    Func<Child(typename Model::Data)> build) {

    return makeStrong<Reducer<Model>>(initial, std::move(reducer), std::move(build));
}

template <typename Model>
inline Child reducer(
    typename Model::Reduce reducer,
    Func<Child(typename Model::Data)> build) {

    return makeStrong<Reducer<Model>>(typename Model::Data{}, std::move(reducer), std::move(build));
}

} // namespace Karm::Ui
