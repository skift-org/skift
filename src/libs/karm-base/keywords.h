#pragma once

#include "bool.h"

namespace Karm {

/* The object should take the ownership of the memory */

struct Move {};

constexpr inline auto MOVE = Move{};

/* The object should make a copy of the memory */

struct Copy {};

constexpr inline auto COPY = Copy{};

/* The object should wrap the memory without doing a copy
   nor taking the ownership */

struct Wrap {};

constexpr inline auto WRAP = Wrap{};

/* The object should be empty initialized */

struct None {
    constexpr None() {}

    explicit operator bool() const {
        return false;
    }
};

constexpr inline auto NONE = None{};

template <typename T>
bool operator==(None, T *ptr) {
    return ptr == nullptr;
}

}; // namespace Karm
