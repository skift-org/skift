#include "prefs.h"

#include "_embed.h"

namespace Karm::App {

Async::Task<Json::Value> MockPrefs::loadAsync(String key, Json::Value defaultValue) {
    auto item = _store.access(key);
    if (item)
        co_return *item;
    co_return defaultValue;
}

Async::Task<> MockPrefs::saveAsync(String key, Json::Value value) {
    _store.put(key, value);
    co_return Ok();
}

Prefs &globalPrefs() {
    return _Embed::globalPrefs();
}

} // namespace Karm::App
