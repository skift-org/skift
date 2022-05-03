#define NO_PRELUDE

#include <karm-base/algos.h>
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

struct Node {
    uint32_t _id = id++;
    Base::Vec<Base::Strong<Node>> _children;

    size_t len() const {
        return _children.len();
    }

    Base::Strong<Node> peek(size_t index) const {
        return _children.peek(index);
    }

    void push(Base::Strong<Node> node) {
        _children.push(node);
    }

    void truncate(size_t len) {
        _children.truncate(len);
    }

    void dump(int indent) {
        for (int i = 0; i < indent * 4; i++) {
            printf(" ");
        }
        printf("Node %d\n", _id);
        for (auto &child : _children) {
            child->dump(indent + 1);
        }
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
    struct Ctx {
        Base::Strong<Node> parent;
        size_t index = 0;
    };

    Base::Vec<Ctx> _stack;
    Base::Strong<Node> _curr = Base::make_strong<Node>();

    Builder _builder;

    Ui(Builder &&builder)
        : _builder(std::forward<Builder>(builder)) {
        _ui = this;
    }

    Ctx &current() {
        return _stack.top();
    }

    Base::Strong<Node> resolve() {
        if (current().index < current().parent->len()) {
            return current().parent->peek(current().index++);
        } else {
            Base::Strong<Node> node = Base::make_strong<Node>();
            current().parent->push(node);
            current().index++;
            return node;
        }
    }

    void begin() {
        _stack.push(Ctx{resolve()});
    }

    void end() {
        current().parent->truncate(current().index + 1);
        _stack.pop();
    }

    inline void scope(auto inner) {
        begin();
        inner();
        end();
    }

    void render() {
        _stack.push(Ctx{_curr});
        _builder();
    }

    void dump() {
        _curr->dump(0);
    }

    int run() {
        return 0;
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

    State(T value) : _value(value) {
    }

    auto value() const -> T const & {
        return _value;
    }

    void update(auto fn) {
        _value = fn(_value);
    }
};

template <typename T>
State<T> useState(T value) {
    State<T> state{value};

    return state;
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

void Window(Children children = {}) {
    ui().scope([&] {
        children();
    });
}

void Widget(Children children = {}) {
    ui().scope([&] {
        children();
    });
}

void VStack(Children children = {}) {
    Widget([&] {
        children();
    });
}

void HStack(Children children = {}) {
    Widget([&] {
        children();
    });
}

void ZStack(Children children = {}) {
    Widget([&] {
        children();
    });
}

template <typename... Ts>
void Label(Karm::Text::Str, Ts &...) {
    Widget([&] {
    });
}

void Button(EventFunc, Children children = {}) {
    Widget([&] {
        children();
    });
}

void Button(Text::Str text, EventFunc event) {
    Button(std::forward<EventFunc>(event), [&] {
        Label(text);
    });
}

void Input(Children children = {}) {
    Widget([&] {
        children();
    });
}

}; // namespace Karm::Ui

using namespace Karm::Ui;

// Simple todo app

int x = 5;

int main(int, char **) {
    Ui app{[] {
        Window([] {
            auto state = useState(0);

            Label(u8"You, clicked {} times", state.value());
            Button(u8"Click me!", [&](Event const &) mutable {
                state.update([](auto &s) {
                    return s + 1;
                });
            });

            for (int i = 0; i < x; i++) {
                Label(u8"Hello");
            }
        });
    }};

    app.render();
    app.dump();

    x = 2;
    app.render();
    app.dump();

    return app.run();
}
