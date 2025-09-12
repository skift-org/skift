module;

#include <stdlib.h>

//
#include <karm-sys/_embed.h>
#include <karm-sys/dir.h>
#include <karm-sys/file.h>

module Karm.App;
import Karm.Core;
import Karm.Ref;
import :_embed;
import :prefs;

namespace Karm::App::_Embed {

struct XdgConfigPrefs : Prefs {
    Ref::Url _url;

    XdgConfigPrefs(Ref::Url url)
        : _url{std::move(url)} {}

    Serde::Value _load() {
        auto data = Sys::readAllUtf8(_url).unwrapOr("{}"s);
        Io::SScan s{data};
        return Json::parse(s).unwrapOr(Serde::Object{});
    }

    Res<> _save(Serde::Value object) {
        auto file = try$(Sys::File::create(_url));
        Io::TextEncoder<> enc{file};
        Io::Emit e{enc};
        return Json::unparse(e, object);
    }

    Async::Task<Serde::Value> loadAsync(String key, Serde::Value defaultValue) {
        auto object = _load();
        auto value = object.get(key);
        co_return Ok(value == NONE ? defaultValue : value);
    }

    Async::Task<> saveAsync(String key, Serde::Value value) {
        auto object = _load();
        object.set(key, value);
        co_return _save(object);
    }
};

static Opt<XdgConfigPrefs> _globalPrefs;

static Res<Ref::Url> _resolveConfigDir() {
    auto* xdgConfigHome = getenv("XDG_CONFIG_HOME");
    if (xdgConfigHome)
        return Ok(Ref::Url::parse(xdgConfigHome) / try$(Sys::_Embed::currentBundle()));

    auto* home = getenv("HOME");
    if (home)
        return Ok(Ref::Url::parse(home) / ".config" / try$(Sys::_Embed::currentBundle()));

    return Error::notFound("could not find XDG_CONFIG_HOME nor HOME");
}

Prefs& globalPrefs() {
    if (not _globalPrefs) {
        auto url = _resolveConfigDir().unwrap("could not resolve preferences directory");
        Sys::Dir::openOrCreate(url).unwrap("could not create preferences directory");
        _globalPrefs = XdgConfigPrefs{url / "configs.json"};
    }
    return *_globalPrefs;
}

} // namespace Karm::App::_Embed
