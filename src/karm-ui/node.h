#pragma once

#include <karm-base/box.h>
#include <karm-base/rc.h>
#include <karm-base/vec.h>
#include <karm-gfx/gfx.h>

#include "event.h"
#include "hook.h"

namespace Karm::Ui {

static uint32_t id = 0;
struct Node : Meta::NoCopy, Meta::NoMove {
    uint32_t _id = id++;

    Vec<Strong<Node>> _children;
    Vec<Box<Hook>> _hooks;

    Node() {
    }

    virtual ~Node() = default;

    /* --- Children --------------------------------------------------------- */

    size_t len() const {
        return _children.len();
    }

    Strong<Node> peek(size_t index) const {
        return _children.peek(index);
    }

    void mount(Strong<Node> node) {
        _children.push(node);
        node->onMount();
    }

    void truncate(size_t len) {
        for (size_t i = len; i < _children.len(); i++) {
            _children.peek(i)->onUnmount();
        }

        _children.truncate(len);
    }

    /* --- Hooks ------------------------------------------------------------ */

    template <typename T, typename... Ts>
    T &hook(size_t index, Ts... ts) {
        if (index == _hooks.len()) {
            _hooks.push(makeBox<T>(ts...));
        }

        return static_cast<T &>(*_hooks.peek(index));
    }

    /* --- Life Cycle ------------------------------------------------------- */

    void onMount() {
        for (auto &hook : _hooks.iter()) {
            hook->onMount();
        }
    }

    void onUnmount() {
        for (auto &hook : _hooks.iter()) {
            hook->onUnmount();
        }
    }

    /* --- Layout & Paint --------------------------------------------------- */

    virtual void onLayout() {
        for (auto &child : _children.iter()) {
            child->onLayout();
        }
    }

    virtual void onPaint(Gfx::Gfx &gfx) {
        for (auto &child : _children.iter()) {
            child->onPaint(gfx);
        }
    }

    /* --- Dump ------------------------------------------------------------- */

    void dump(int indent) {
        for (int i = 0; i < indent * 4; i++) {
            printf(" ");
        }

        printf("Node %d: ", _id);

        for (auto &hook : _hooks.iter()) {
            printf("%s ", (char const *)hook->desc().buf());
        }

        printf("\n");

        for (auto &child : _children.iter()) {
            child->dump(indent + 1);
        }
    }
};

} // namespace Karm::Ui
