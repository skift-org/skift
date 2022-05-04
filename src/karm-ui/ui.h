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

    Ctx &current() {
        return _stack.top();
    }

    Base::Strong<Node> reconcile() {
        if (current().child < current().parent->len()) {
            return current().parent->peek(current().child++);
        } else {
            Base::Strong<Node> node = Base::makeStrong<Node>();
            current().parent->mount(node);
            current().child++;
            return node;
        }
    }

    void begin() {
        _stack.push(Ctx{reconcile()});
    }

    Node &end() {
        Node &node = *_stack.top().parent;
        current().parent->truncate(current().child + 1);
        _stack.pop();
        return node;
    }

    inline Node &group(auto inner) {
        begin();
        inner();
        return end();
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

Node &group(Children children = {}) {
    return ui().group([&] {
        children();
    });
}

} // namespace Karm::Ui
