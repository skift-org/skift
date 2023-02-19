#pragma once

#include <hal/vmm.h>
#include <karm-base/array.h>
#include <karm-base/rc.h>

namespace Hjert::Core {

struct Object {
    size_t _id = 0;

    size_t id() const { return _id; }

    virtual ~Object() = default;
};

template <typename Crtp>
struct _Object : public Object {};

using Slot = Opt<Strong<Object>>;

struct Node : public _Object<Node> {
};

template <size_t B>
struct _Node : public Node {
    Array<Slot, (1 << B) - 1> _slots;
};

struct Mem : public Object {
};

struct Map {
    Hal::VmmRange vrange;
    size_t off;
    Strong<Mem> mem;
};

struct Space : public Object {

    virtual void switchTo() = 0;
};

struct Io : public Object {
};

struct Task : public Object {
    Strong<Space> _space;
    Strong<Node> _root;
};

} // namespace Hjert::Core
