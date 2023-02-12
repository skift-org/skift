#pragma once

#include <json/json.h>
#include <karm-base/string.h>
#include <karm-base/vec.h>

namespace Loader {

struct File {
    String path;
    Json::Value props;

    static Result<File> fromJson(Json::Value const &json) {
        if (not json.isObject()) {
            return Error{"expected object"};
        }

        File file = {};

        file.path = try$(json.get("path").take<String>());
        file.props = json.get("props");

        return file;
    }
};

struct Entry {
    String name;
    File kernel;
    Vec<File> files;

    static Result<Entry> fromJson(Json::Value const &json) {
        if (not json.isObject()) {
            return Error{"expected object"};
        }

        Entry entry = {};

        entry.name = try$(json.get("name").take<String>());
        auto kernelJson = try$(json.get("kernel").take<Json::Object>());
        entry.kernel = try$(File::fromJson(kernelJson));

        auto filesJson = try$(json.get("files").take<Json::Array>());
        for (auto const &fileJson : filesJson) {
            auto file = try$(File::fromJson(fileJson));
            entry.files.pushBack(file);
        }

        return entry;
    }
};

struct Configs {
    Vec<Entry> entries;

    static Result<Configs> fromJson(Json::Value const &json) {
        if (not json.isArray()) {
            return Error{"expected array"};
        }

        Configs configs = {};

        auto entriesJson = try$(json.take<Json::Array>());
        for (auto const &entryJson : entriesJson) {
            auto entry = try$(Entry::fromJson(entryJson));
            configs.entries.pushBack(entry);
        }

        return configs;
    }
};

} // namespace Loader
