#pragma once

#include <karm-base/var.h>

#include "react.h"

namespace Karm::Ui {

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
        this->shouldRebuild();
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

    return makeStrong<Reducer<Model>>({}, std::move(reducer), std::move(build));
}

} // namespace Karm::Ui
