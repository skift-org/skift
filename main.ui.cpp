#define NO_PRELUDE

#include <karm-base/box.h>
#include <karm-base/cons.h>
#include <karm-base/func.h>
#include <karm-base/rc.h>
#include <karm-base/tuple.h>
#include <karm-base/vec.h>
#include <karm-meta/id.h>

namespace Karm::Ui {

struct Node : public Base::Vec<Base::OptStrong<Node>> {
    void dump(int indent) {
        for (int i = 0; i < indent; i++) {
            printf(" ");
        }
        printf("Node\n");
        for (auto &child : *this) {
            child.visit([&](auto &node) {
                node->dump(indent + 2);
            });
        }
    }
};

struct Ctx {
    Base::OptStrong<Node> _curr = Base::make_strong<Node>();

    void scope(auto inner) {
        auto prev = _curr.unwrap();
        _curr = Base::make_strong<Node>();
        inner();
        prev->push(std::move(_curr));
        _curr = prev;
    }
};

void window(Ctx &ctx, auto inner) {
    ctx.scope([&] {
        inner();
    });
}

void vstack(Ctx &ctx, auto inner) {
    ctx.scope([&] {
        inner();
    });
}

void hstack(Ctx &ctx, auto inner) {
    ctx.scope([&] {
        inner();
    });
}

void button(Ctx &ctx, auto inner) {
    ctx.scope([&] {
        inner();
    });
}

void text(Ctx &ctx, auto inner) {
    ctx.scope([&] {
        inner();
    });
}

void input(Ctx &ctx, auto inner) {
    ctx.scope([&] {
        inner();
    });
}

}; // namespace Karm::Ui

using namespace Karm::Ui;

// Simple todo app

static void app(Ctx &ctx) {
    window(ctx, [&] {
        vstack(ctx, [&] {
            text(ctx, [&] {
            });
            hstack(ctx, [&] {
                input(ctx, [&] {
                });
                button(ctx, [&] {
                });
            });
            hstack(ctx, [&] {
                button(ctx, [&] {
                });
                button(ctx, [&] {
                });
            });
        });
    });
}

int main(int, char **) {
    Ctx ctx;
    app(ctx);
    ctx._curr.with([](auto &node) {
        node->dump(0);
    });

    return 0;
}
