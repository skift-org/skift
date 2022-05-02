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

static struct Ui *_ui = {};

struct Ui {
    Builder _builder;

    Base::Strong<Node> _curr = Base::make_strong<Node>();
    Base::OptStrong<Node> _wip;

    Ui(Builder &&builder)
        : _builder(std::forward<Builder>(builder)) {
        _ui = this;
    }

    inline Node &scope(auto inner) {
        auto parent = _wip.unwrap();
        _wip = Base::make_strong<Node>();
        inner();
        parent->push_car(_wip.unwrap());
        _wip = parent;
        return *_wip.unwrap();
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

Ui &ui() {
    return *_ui;
}

struct Event {};

using EventFunc = Base::Func<void(Event const &)>;

template <typename T>
struct State {
    T _value;

    auto value() const -> T const & {
        return _value;
    }

    void update(auto fn) {
        _value = fn(_value);
    }
};

template <typename T>
State<T> &useState(T value) {
    return {};
}

template <typename T>
struct Atom {
    T _value;

    auto value() const -> T const & {
        return _value;
    }

    void update(auto fn) {
        _value = fn(_value);
    }
};

template <typename T>
Atom<T> useAtom() {
    return {};
}

void useEffect(Base::Func<Base::Func<void()>()>);

Node &Window(Children children = {}) {
    return ui().scope([&] {
        children();
    });
}

Node &Widget(Children children = {}) {
    return ui().scope([&] {
        children();
    });
}

Node &VStack(Children children = {}) {
    return Widget([&] {
        children();
    });
}

Node &HStack(Children children = {}) {
    return Widget([&] {
        children();
    });
}

Node &ZStack(Children children = {}) {
    return Widget([&] {
        children();
    });
} 

template <typename... Ts>
Node &Label(Karm::Text::Str, Ts &...) {
    return Widget([&] {
    });
}

Node &Button(EventFunc, Children children = {}) {
    return Widget([&] {
        children();
    });
}

Node &Button(Text::Str text, EventFunc event) {
    return Button(std::forward<EventFunc>(event), [&] {
        Label(text);
    });
}

Node &Input(Children children = {}) {
    return Widget([&] {
        children();
    });
}

}; // namespace Karm::Ui

using namespace Karm::Ui;

// Simple todo app

int main(int, char **) {
    Ui app{[] {
        Window([] {
            auto state = useState(0);

            Label(u8"You, clicked {} times", state.value());
            Button(u8"Click me!", [state]() mutable {
                state.update([](auto &s) {
                    return s + 1;
                });
            });
        });
    }};

    return app.run();
}
