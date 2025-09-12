export module Karm.App:prefs;

import Karm.Core;
import :_embed;

namespace Karm::App {

export struct Prefs {
    virtual ~Prefs() = default;

    virtual Async::Task<Serde::Value> loadAsync(String key, Serde::Value defaultValue = NONE) = 0;

    virtual Async::Task<> saveAsync(String key, Serde::Value value) = 0;
};

export struct MockPrefs : Prefs {
    Serde::Object _store;

    Async::Task<Serde::Value> loadAsync(String key, Serde::Value defaultValue = NONE) override {
        auto item = _store.access(key);
        if (item)
            co_return *item;
        co_return defaultValue;
    }

    Async::Task<> saveAsync(String key, Serde::Value value) override {
        _store.put(key, value);
        co_return Ok();
    }
};

export Prefs& globalPrefs() {
    return _Embed::globalPrefs();
}

} // namespace Karm::App
