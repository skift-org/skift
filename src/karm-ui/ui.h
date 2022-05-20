#pragma once

#include <karm-base/func.h>
#include <karm-base/rc.h>
#include <karm-base/vec.h>

#include "node.h"

namespace Karm::Ui {

using Builder = Func<void()>;

struct Children : public Opt<Builder> {
    using Opt<Builder>::Opt;

    Children(auto &&builder) : Opt<Builder>(std::forward<decltype(builder)>(builder)) {
    }

    void operator()() {
        with([&](auto &f) {
            f();
        });
    }
};

static struct Ctx *_ctx = {};

struct Ctx {
    struct Scope {
        Strong<Node> parent;
        size_t child = 0;
        size_t hook = 0;
    };

    Vec<Scope> _stack;
    Strong<Node> _curr = makeStrong<Node>();

    Builder _builder;

    Ctx(Builder &&builder)
        : _builder(std::forward<Builder>(builder)) {
        _ctx = this;
    }

    Scope &currentScope() {
        return _stack.top();
    }

    Node &currentNode() {
        return *currentScope().parent;
    }

    Strong<Node> reconcile() {
        if (currentScope().child < currentNode().len()) {
            return currentNode().peek(currentScope().child++);
        } else {
            Strong<Node> node = makeStrong<Node>();
            currentNode().mount(node);
            currentScope().child++;
            return node;
        }
    }
    void push() {
        _stack.push(Scope{reconcile()});
    }

    Node &pop() {
        Node &node = *_stack.top().parent;
        currentNode().truncate(currentScope().child + 1);
        _stack.pop();
        return node;
    }

    inline Node &group(auto inner) {
        push();
        inner();
        return pop();
    }

    template <typename T, typename... Ts>
    T &hook(Ts... ts) {
        return currentNode().hook<T>(currentScope().hook++, ts...);
    }

    void rebuild() {
        _stack.push(Scope{_curr});
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

Ctx &ctx() {
    return *_ctx;
}

} // namespace Karm::Ui
