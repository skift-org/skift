#define NO_PRELUDE

#include <karm-base/box.h>
#include <karm-base/cons.h>
#include <karm-base/func.h>
#include <karm-base/rc.h>
#include <karm-base/tuple.h>
#include <karm-base/var.h>
#include <karm-base/vec.h>
#include <karm-meta/id.h>
#include <karm-text/str.h>

namespace Karm::Ui {

static uint32_t id = 0;
struct Node : public Base::Cons<Base::OptStrong<Node>, Base::OptStrong<Node>> {
    uint32_t _id = id++;

    void push_cdr(Base::Strong<Node> &node) {
        if (!cdr.with([&](auto &n) {
                n->push_cdr(node);
            })) {
            cdr = node;
        }
    }

    void push_car(Base::Strong<Node> &node) {
        if (!car.with([&](auto &n) {
                n->push_cdr(node);
            })) {
            car = node;
        }
    }

    void dump(int indent) {
        for (int i = 0; i < indent; i++) {
            printf(" ");
        }
        printf("Node %d\n", _id);
        car.with([&](auto const &children) {
            children->dump(indent + 1);
        });

        cdr.with([&](auto const &sibling) {
            sibling->dump(indent);
        });
    }
};

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

static struct Ctx *_ctx = {};

struct Ctx {
    Builder _builder;

    Base::Strong<Node> _curr = Base::make_strong<Node>();
    Base::OptStrong<Node> _wip;

    Ctx(Builder &&builder)
        : _builder(std::forward<Builder>(builder)) {
        _ctx = this;
    }

    inline void scope(auto inner) {
        auto parent = _wip.unwrap();
        _wip = Base::make_strong<Node>();
        inner();
        parent->push_car(_wip.unwrap());
        _wip = parent;
    }

    Base::OptStrong<Node> reconcile(Base::OptStrong<Node> optCurr, Base::OptStrong<Node> optWip) {
        if (!optWip) {
            return Base::NONE;
        }

        if (!optCurr) {
            return optWip;
        }

        Base::Strong<Node> curr = optCurr.unwrap();
        Base::Strong<Node> wip = optWip.unwrap();

        if (wip->car) {
            if (curr->car) {
                curr->car = reconcile(curr->car, wip->car);
            } else {
                curr->car = wip->car;
            }
        }

        if (wip->cdr) {
            if (curr->cdr) {
                curr->cdr = reconcile(curr->cdr, wip->cdr);
            } else {
                curr->cdr = wip->cdr;
            }
        }

        return curr;
    }

    Base::Strong<Node> reconcile() {
        return reconcile(_curr, _wip).unwrap();
    }

    void render() {
        _wip = Base::make_strong<Node>();
        _builder();
        _curr = reconcile();
    }

    void dump() {
        _curr->dump(0);
    }
};

Ctx &ctx() {
    return *_ctx;
}

void Window(Children children = {}) {
    ctx().scope([&] {
        children();
    });
}

void VStack(Children children = {}) {
    ctx().scope([&] {
        children();
    });
}

void HStack(Children children = {}) {
    ctx().scope([&] {
        children();
    });
}

void Button(Children children = {}) {
    ctx().scope([&] {
        children();
    });
}

void Label(Karm::Text::Str, Children children = {}) {
    ctx().scope([&] {
        children();
    });
}

void Input(Children children = {}) {
    ctx().scope([&] {
        children();
    });
}

}; // namespace Karm::Ui

using namespace Karm::Ui;

// Simple todo app

int state = 2;
int main(int, char **) {
    Ctx app{[] {
        Window([] {
            VStack([] {
                Label(u8"Hello, world!");
                HStack([] {
                    for (int i = 0; i < state; i++) {
                        Button();
                    }
                });
                HStack([] {
                    Button();
                    Button();
                });
            });
        });
    }};

    app.render();
    app.dump();

    printf("\nState change:\n");
    state = 5;
    app.render();
    app.dump();

    return 0;
}
