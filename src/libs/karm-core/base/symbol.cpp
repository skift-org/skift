module;

#include <karm-core/macros.h>

export module Karm.Core:base.symbol;

import :base.rc;
import :base.set;
import :base.hash;
import :base.string;

namespace Karm {

struct _SymbolBuf {
    using Inner = char;
    usize _len;
    [[no_unique_address]] char _buf[0];

    usize len() const {
        return _len;
    }

    char const* buf() const {
        return _buf;
    }

    char const& operator[](usize i) const {
        if (i >= _len) [[unlikely]]
            panic("index out of bounds");
        return _buf[i];
    }

    char* buf() {
        return _buf;
    }

    char& operator[](usize i) {
        if (i >= _len) [[unlikely]]
            panic("index out of bounds");
        return _buf[i];
    }

    static Rc<_SymbolBuf> from(Str str) {
        using _StorageCell = Cell<NoLock, _SymbolBuf>;
        Rc<_SymbolBuf> buf = {
            MOVE,
            new (reinterpret_cast<_StorageCell*>(new u8[sizeof(_StorageCell) + str.len() + 1])) _StorageCell(str.len()),
        };
        copy(str, mutSub(*buf));
        buf->_buf[str.len()] = 0;
        return buf;
    }

    bool operator==(_SymbolBuf const& other) const {
        return Str{*this} == Str{other};
    }

    bool operator==(Str const& other) const {
        return Str(*this) == other;
    }

    u64 hash() const {
        return Karm::hash(Str{*this});
    }
};

/// A symbol is a unique string that is interned in a global registry.
/// It is used to represent identifiers in a way that allows for fast comparisons and lookups.
/// Symbols are immutable and can be compared by pointer equality.
/// They are typically used for identifiers in languages, such as HTML tags, attributes, and other
/// names that are used frequently and need to be compared often.
export struct Symbol {
    Rc<_SymbolBuf> _buf;

    /// Symbol representing an empty string.
    static Symbol EMPTY;

    static Symbol from(Str str);

    Str str() const {
        return Str(*_buf);
    }

    bool operator==(Symbol const& other) const {
        return _buf._cell == other._buf._cell;
    }

    bool operator==(Str const& other) const {
        return str() == other;
    }

    auto operator<=>(Symbol const& other) const {
        return str() <=> other.str();
    }

    explicit operator bool() const {
        return str().len() > 0;
    }
};

inline Symbol Symbol::EMPTY = from("");

static Set<Rc<_SymbolBuf>>& _symboleRegistry() {
    static Set<Rc<_SymbolBuf>> _registry;
    return _registry;
}

Symbol Symbol::from(Str str) {
    auto& registry = _symboleRegistry();
    registry.ensureForInsert();
    auto* slot = registry.lookup(str);
    if (slot and slot->state == Set<Rc<_SymbolBuf>>::State::USED) {
        return {slot->unwrap()};
    }

    auto buf = _SymbolBuf::from(str);
    registry.put(buf);
    return {buf};
}

} // namespace Karm

export constexpr Karm::Symbol operator""_sym(char const* buf, Karm::usize len) {
    return Karm::Symbol::from({buf, len});
}

export constexpr Karm::Symbol operator""_sym(char const* buf) {
    return Karm::Symbol::from(buf);
}
