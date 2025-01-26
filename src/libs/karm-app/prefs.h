#pragma once

#include <karm-async/task.h>
#include <karm-json/values.h>

namespace Karm::App {

struct Prefs {
    virtual ~Prefs() = default;

    virtual Async::Task<Json::Value> loadAsync(String key, Json::Value defaultValue = NONE) = 0;

    virtual Async::Task<> saveAsync(String key, Json::Value value) = 0;
};

struct MockPrefs : public Prefs {
    Json::Object _store;

    Async::Task<Json::Value> loadAsync(String key, Json::Value defaultValue) override;

    Async::Task<> saveAsync(String key, Json::Value value) override;
};

Prefs& globalPrefs();

} // namespace Karm::App
