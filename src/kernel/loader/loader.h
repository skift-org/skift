#pragma once

#include <json/json.h>
#include <karm-main/base.h>
#include <karm-media/icon.h>
#include <karm-media/image.h>

namespace Loader {

struct File {
    String path;
    Json::Value props;

    static Res<File> fromJson(Json::Value const &json) {
        if (not json.isObject()) {
            return Error::invalidInput("expected object");
        }

        File file = {};

        file.path = try$(json.get("path").take<String>());
        file.props = json.get("props");

        return Ok(file);
    }
};

struct Entry {
    Var<None, Mdi::Icon, Media::Image> icon = NONE;
    String name;
    File kernel;
    Vec<File> files;

    static Res<Entry> fromJson(Json::Value const &json) {
        if (not json.isObject()) {
            return Error::invalidInput("expected object");
        }

        Entry entry = {};

        auto maybeIcon = json.get("icon").take<String>();
        if (maybeIcon) {
            auto maybeImage = Media::loadImage(*maybeIcon);
            if (not maybeImage) {
                entry.icon = Mdi::byName(*maybeIcon).unwrap();
            } else {
                entry.icon = maybeImage.unwrap();
            }
        }

        entry.name = try$(json.get("name").take<String>());
        auto kernelJson = try$(json.get("kernel").take<Json::Object>());
        entry.kernel = try$(File::fromJson(kernelJson));

        auto filesJson = try$(json.get("files").take<Json::Array>());
        for (auto const &fileJson : filesJson) {
            auto file = try$(File::fromJson(fileJson));
            entry.files.pushBack(file);
        }

        return Ok(entry);
    }
};

struct Configs {
    Vec<Entry> entries;

    static Res<Configs> fromJson(Json::Value const &json) {
        if (not json.isObject()) {
            return Error::invalidInput("expected array");
        }

        Configs configs = {};

        auto entriesJson = try$(json.get("entries").take<Json::Array>());
        for (auto const &entryJson : entriesJson) {
            auto entry = try$(Entry::fromJson(entryJson));
            configs.entries.pushBack(entry);
        }

        return Ok(configs);
    }
};

Res<> showMenu(Ctx &ctx, Configs const &c);

Res<> loadEntry(Entry const &);

} // namespace Loader
