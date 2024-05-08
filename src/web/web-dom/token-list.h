#pragma once

#include <karm-base/string.h>
#include <karm-base/vec.h>

namespace Web::Dom {

// https://dom.spec.whatwg.org/#domtokenlist
struct TokenList {
    Vec<String> _tokens;

    usize length() const {
        return _tokens.len();
    }

    Opt<String> item(usize index) const {
        if (index >= _tokens.len())
            return NONE;
        return _tokens[index];
    }

    bool contains(Str token) const {
        return _tokens.contains(token);
    }

    void add(Str token) {
        if (not _tokens.contains(token))
            _tokens.pushBack(token);
    }

    void remove(Str token) {
        _tokens.removeAll(token);
    }

    bool toggle(Str token) {
        if (_tokens.contains(token)) {
            _tokens.removeAll(token);
            return false;
        } else {
            _tokens.pushBack(token);
            return true;
        }
    }

    bool replace(Str oldToken, Str newToken) {
        if (not _tokens.contains(oldToken))
            return false;
        _tokens.removeAll(oldToken);
        _tokens.pushBack(newToken);
        return true;
    }
};

} // namespace Web::Dom
