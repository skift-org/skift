#pragma once

#include "ptr.h"

namespace Karm::Gc {

struct Cell {
    Cell* _next = nullptr;

    virtual ~Cell() = default;
};

template <typename T>
struct _Cell : public Cell {
    T store;

    template <typename... Args>
    _Cell(Args&&... args)
        : store{std::forward<Args>(args)...} {
    }
};

struct Heap : Meta::Pinned {
    Cell* _root = nullptr;
    Cell* _last = nullptr;

    void destroyAll() {
        while (_root) {
            auto* next = _root->_next;
            delete _root;
            _root = next;
        }

        _root = nullptr;
        _last = nullptr;
    }

    ~Heap() {
        destroyAll();
    }

    template <typename T, typename... Args>
    Ref<T> alloc(Args&&... args) lifetimebound {
        auto* cell = new _Cell<T>(std::forward<Args>(args)...);

        if (not _root) {
            _root = cell;
            _last = cell;
        } else {
            _last->_next = cell;
            _last = cell;
        }

        return {MOVE, &cell->store};
    }
};

} // namespace Karm::Gc
