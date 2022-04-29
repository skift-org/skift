#define NO_PRELUDE

#include <karm-base/box.h>
#include <karm-base/cons.h>
#include <karm-base/func.h>
#include <karm-base/rc.h>
#include <karm-base/tuple.h>
#include <karm-base/vec.h>
#include <karm-meta/id.h>

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

using Builder = Base::Func<void(struct Ctx &)>;

struct Ctx {
    Builder _builder;

    bool _update = false;
    Base::Strong<Node> _curr = Base::make_strong<Node>();
    Base::OptStrong<Node> _wip;

    Ctx(Builder &&builder)
        : _builder(std::forward<Builder>(builder)) {
    }

    void scope(auto inner) {
        if (!_update) {
            auto parent = _curr;
            _curr = Base::make_strong<Node>();
            inner();
            parent->push_car(_curr);
            _curr = parent;
        } else {
            auto parent = _wip.unwrap();
            _wip = Base::make_strong<Node>();
            inner();
            parent->push_car(_wip.unwrap());
            _wip = parent;
        }
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
        if (_update) {
            _wip = Base::make_strong<Node>();
            _builder(*this);
            _curr = reconcile();
        } else {
            _builder(*this);
            _update = true;
        }
    }

    void dump() {
        _curr->dump(0);
    }
};

void Window(Ctx &ctx, auto inner) {
    ctx.scope([&] {
        inner();
    });
}

void VStack(Ctx &ctx, auto inner) {
    ctx.scope([&] {
        inner();
    });
}

void HStack(Ctx &ctx, auto inner) {
    ctx.scope([&] {
        inner();
    });
}

void Button(Ctx &ctx, auto inner) {
    ctx.scope([&] {
        inner();
    });
}

void Text(Ctx &ctx, auto inner) {
    ctx.scope([&] {
        inner();
    });
}

void Input(Ctx &ctx, auto inner) {
    ctx.scope([&] {
        inner();
    });
}

}; // namespace Karm::Ui

using namespace Karm::Ui;

// Simple todo app

int state = 2;

int main(int, char **) {
    Ctx app{[](auto &ctx) {
        Window(ctx, [&] {
            VStack(ctx, [&] {
                Text(ctx, [&] {
                });
                HStack(ctx, [&] {
                    for (int i = 0; i < state; i++) {
                        Button(ctx, [&] {
                        });
                    }
                });
                HStack(ctx, [&] {
                    Button(ctx, [&] {
                    });
                    Button(ctx, [&] {
                    });
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
