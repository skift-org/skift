#pragma once

#include <karm-base/func.h>
#include <karm-base/rc.h>
#include <karm-base/vec.h>

#include "node.h"

namespace Karm::Ui {

using Builder = Base::Func<void()>;

struct Children : public Base::Opt<Builder> {
    using Base::Opt<Builder>::Opt;

    Children(auto &&builder) : Base::Opt<Builder>(std::forward<decltype(builder)>(builder)) {
    }

    void operator()() {
        with([&](auto &f) {
            f();
        });
    }
};

static struct Ui *_ui = {};

struct Ui {
    struct Ctx {
        Base::Strong<Node> parent;
        size_t child = 0;
        size_t hook = 0;
    };

    Base::Vec<Ctx> _stack;
    Base::Strong<Node> _curr = Base::makeStrong<Node>();

    Builder _builder;

    Ui(Builder &&builder)
        : _builder(std::forward<Builder>(builder)) {
        _ui = this;
    }

    Ctx &currentCtx() {
        return _stack.top();
    }

    Node &currentNode() {
        return *currentCtx().parent;
    }

    Base::Strong<Node> reconcile() {
        if (currentCtx().child < currentNode().len()) {
            return currentNode().peek(currentCtx().child++);
        } else {
            Base::Strong<Node> node = Base::makeStrong<Node>();
            currentNode().mount(node);
            currentCtx().child++;
            return node;
        }
    }
    void begin() {
        _stack.push(Ctx{reconcile()});
    }

    Node &end() {
        Node &node = *_stack.top().parent;
        currentNode().truncate(currentCtx().child + 1);
        _stack.pop();
        return node;
    }

    inline Node &group(auto inner) {
        begin();
        inner();
        return end();
    }

    template <typename T, typename... Ts>
    T &hook(Ts... ts) {
        return currentNode().hook<T>(currentCtx().hook++, ts...);
    }

    void rebuild() {
        _stack.push(Ctx{_curr});
        _builder();
    }

    void shouldRebuild() {
    }

    void dump() {
        _curr->dump(0);
    }

    int run() {
        rebuild();
        dump();
        return 0;
    }
};

Ui &ui() {
    return *_ui;
}

} // namespace Karm::Ui
