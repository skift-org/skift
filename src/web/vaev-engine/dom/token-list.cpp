export module Vaev.Engine:dom.token_list;

import Karm.Core;

using namespace Karm;

namespace Vaev::Dom {

// https://dom.spec.whatwg.org/#domtokenlist
export struct TokenList {
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
        return ::contains(_tokens, token);
    }

    void add(Str token) {
        if (not ::contains(_tokens, token))
            _tokens.pushBack(token);
    }

    void remove(Str token) {
        _tokens.removeAll(token);
    }

    bool toggle(Str token) {
        if (::contains(_tokens, token)) {
            _tokens.removeAll(token);
            return false;
        }
        _tokens.pushBack(token);
        return true;
    }

    bool replace(Str oldToken, Str newToken) {
        if (not ::contains(_tokens, oldToken))
            return false;
        _tokens.removeAll(oldToken);
        _tokens.pushBack(newToken);
        return true;
    }
};

} // namespace Vaev::Dom
