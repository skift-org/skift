#include <karm-app/_embed.h>
#include <karm-app/prefs.h>
#include <karm-json/parse.h>
#include <karm-logger/logger.h>
#include <karm-mime/url.h>
#include <karm-pkg/_embed.h>
#include <karm-sys/dir.h>
#include <karm-sys/file.h>

namespace Karm::App::_Ember {

struct XdgConfigPrefs : public Prefs {
    Mime::Url _url;

    XdgConfigPrefs(Mime::Url url)
        : _url{std::move(url)} {}

    Json::Value _load() {
        auto data = Sys::readAllUtf8(_url).unwrapOr("{}"s);
        Io::SScan s{data};
        return Json::parse(s).unwrapOr(Json::Object{});
    }

    Res<> _save(Json::Value object) {
        auto file = try$(Sys::File::create(_url));
        Io::TextEncoder<> enc{file};
        Io::Emit e{enc};
        return Json::unparse(e, object);
    }

    Async::Task<Json::Value> loadAsync(String key, Json::Value defaultValue) {
        auto object = _load();
        auto value = object.get(key);
        co_return Ok(value == NONE ? defaultValue : value);
    }

    Async::Task<> saveAsync(String key, Json::Value value) {
        auto object = _load();
        object.set(key, value);
        co_return _save(object);
    }
};

static Opt<XdgConfigPrefs> _globalPrefs;

static Res<Mime::Url> _resolveConfigDir() {
    auto* xdgConfigHome = getenv("XDG_CONFIG_HOME");
    if (xdgConfigHome)
        return Ok(Mime::Url::parse(xdgConfigHome) / try$(Pkg::_Embed::currentBundle()));

    auto* home = getenv("HOME");
    if (home)
        return Ok(Mime::Url::parse(home) / ".config" / try$(Pkg::_Embed::currentBundle()));

    return Error::notFound("could not find XDG_CONFIG_HOME nor HOME");
}

Prefs& globalPrefs() {
    if (not _globalPrefs) {
        auto url = _resolveConfigDir().unwrap("could not resolve preferences directory");
        Sys::Dir::openOrCreate(url).unwrap("could not create preferences directory");
        logInfo("preferences stored at {}", url);
        _globalPrefs = XdgConfigPrefs{url / "configs.json"};
    }
    return *_globalPrefs;
}

} // namespace Karm::App::_Ember
