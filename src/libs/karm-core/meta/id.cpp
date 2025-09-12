export module Karm.Core:meta.id;

import :base.base;
import :meta.traits;

namespace Karm::Meta {

export using Id = usize;

export template <typename T>
constexpr Id idOf() {
    char const* cstr = __PRETTY_FUNCTION__;
    usize len = sizeof(__PRETTY_FUNCTION__);
    usize hash = 0uz;
    for (char const* b = cstr; b < cstr + len; b++)
        hash = (1000003 * hash) ^ *b;
    hash ^= len;
    return hash;
}

} // namespace Karm::Meta
