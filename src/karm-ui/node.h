#pragma once

#include <karm-base/box.h>
#include <karm-base/rc.h>
#include <karm-base/vec.h>
#include <karm-gfx/gfx.h>

#include "event.h"
#include "hook.h"

namespace Karm::Ui {

static uint32_t id = 0;

struct NoCopy {
    NoCopy() = default;
    NoCopy(NoCopy const &) = delete;
    NoCopy &operator=(NoCopy const &) = delete;
};

struct NoMove {
    NoMove() = default;
    NoMove(NoMove &&) = delete;
    NoMove &operator=(NoMove &&) = delete;
};

struct Node {
    uint32_t _id = id++;

    Base::Vec<Base::Strong<Node>> _children;
    Base::Vec<Base::Box<Hook>> _hooks ;

    Node() {
    }

    virtual ~Node() = default;

    /* --- Children --------------------------------------------------------- */

    size_t len() const {
        return _children.len();
    }

    Base::Strong<Node> peek(size_t index) const {
        return _children.peek(index);
    }

    void mount(Base::Strong<Node> node) {
        _children.push(node);
        node->on_mount();
    }

    void truncate(size_t len) {
        for (size_t i = len; i < _children.len(); i++) {
            _children.peek(i)->on_unmount();
        }

        _children.truncate(len);
    }

    /* --- Hooks ------------------------------------------------------------ */

    template <typename T, typename... Ts>
    T &hook(size_t index, Ts... ts) {
        if (index == _hooks.len()) {
            _hooks.push(Base::make_box<T>(ts...));
        }

        return *_hooks.peek(index);
    }

    /* --- Life Cycle ------------------------------------------------------- */

    void on_mount() {
        for (auto &hook : _hooks) {
            hook->on_mount();
        }
    }

    void on_unmount() {
        for (auto &hook : _hooks) {
            hook->on_unmount();
        }
    }

    /* --- Layout & Paint --------------------------------------------------- */

    virtual void on_layout() {
        for (auto &child : _children) {
            child->on_layout();
        }
    }

    virtual void on_paint(Gfx::Gfx &gfx) {
        for (auto &child : _children) {
            child->on_paint(gfx);
        }
    }

    /* --- Dump ------------------------------------------------------------- */

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

} // namespace Karm::Ui
