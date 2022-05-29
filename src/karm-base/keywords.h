#pragma once

namespace Karm {

struct Adopt {};

struct Copy {};

struct Wrap {};

struct None {};

/* The object should take the ownership of the memory */
constexpr inline auto ADOPT = Adopt{};

/* The object should make a copy of the memory */
constexpr inline auto COPY = Copy{};

/* The object should wrap the memory without doing a copy
   nor taking the ownership */
constexpr inline auto WRAP = Wrap{};

/* The object should be empty initialized */
constexpr inline auto NONE = None{};

template <typename T>
bool operator==(None, T *ptr) {
    return ptr == nullptr;
}

}; // namespace Karm
