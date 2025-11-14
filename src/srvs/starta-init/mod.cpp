module;

#include "karm-core/macros.h"

export module Strata.Init;

import Karm.Core;
import Karm.Sys;
import Karm.Logger;
import Karm.Ref;

using namespace Karm;

namespace Strata::Init {

struct UnitManifest {
    Symbol id;
    Opt<String> name = NONE;
    Opt<String> description = NONE;
    Vec<Symbol> requires_ = {};
    Vec<Symbol> provides = {};
    Vec<Symbol> injects = {};
    Opt<String> onStart = NONE;
    Opt<String> onStop = NONE;

    static Res<UnitManifest> fromJson(Serde::Value const& val) {
        auto object = val.isObject();
        if (not object)
            return Error::invalidData("expected unit-manifest object");

        auto id = object->tryGet("id"s).unwrapOr(NONE);
        if (not id.isStr())
            return Error::invalidData("missing 'id' in unit-manifest");

        auto res = UnitManifest{
            .id = Symbol::from(id.asStr()),
        };

        auto provides = object->tryGet("provides"s).unwrapOr(NONE);
        if (provides.isArray()) {
            for (auto& provide : provides.asArray()) {
                res.provides.pushBack(Symbol::from(provide.asStr()));
            }
        }

        auto requires_ = object->tryGet("requires"s).unwrapOr(NONE);
        if (requires_.isArray()) {
            for (auto& require : requires_.asArray()) {
                res.requires_.pushBack(Symbol::from(require.asStr()));
            }
        }

        auto injects = object->tryGet("injects"s).unwrapOr(NONE);
        if (injects.isArray()) {
            for (auto& require : injects.asArray()) {
                res.injects.pushBack(Symbol::from(require.asStr()));
            }
        }

        auto onStart = object->tryGet("onStart"s).unwrapOr(NONE);
        if (onStart.isStr())
            res.onStart = onStart.asStr();

        auto onStop = object->tryGet("onStop"s).unwrapOr(NONE);
        if (onStop.isStr())
            res.onStop = onStop.asStr();


        return Ok(std::move(res));

    }

    void repr(Io::Emit& emit) const {
        emit("(unit-manifest ");
        emit("id: {#} ", id);
        emit("requires: {} ", requires_);
        emit("provides: {} ", provides);
        emit("injects: {} ", injects);
        emit("onStart: {#} ", onStart);
        emit("onStop: {#}", onStop);
        emit(")");
    }
};

export struct UnitManager {
    Res<Map<Symbol, UnitManifest>> listUnits() {
        Map<Symbol, UnitManifest> res;
        for (auto const& bundle : try$(Sys::Bundle::installed())) {
            auto dirUrl = bundle.url() / "system/units"s;
            auto maybeDir = Sys::Dir::open(dirUrl);
            if (not maybeDir)
                continue;

            for (auto const& diren : maybeDir.unwrap().entries()) {
                if (diren.type != Sys::Type::FILE)
                    continue;

                auto fileUrl = dirUrl / diren.name;
                if (fileUrl.path.suffix() != "unit")
                    continue;

                logInfo("Loading {}...", dirUrl);

                auto maybeManifestContent = Sys::readAllUtf8(fileUrl);
                if (not maybeManifestContent) {
                    logError("Failed to read manifest at {}: {}", fileUrl, maybeManifestContent);
                    continue;
                }

                auto maybeManifestJson = Json::parse(maybeManifestContent.unwrap());
                if (not maybeManifestJson) {
                    logError("Failed to parse manifest at {}: {}", fileUrl, maybeManifestJson);
                    continue;
                }

                auto maybeManifest = UnitManifest::fromJson(maybeManifestJson.unwrap());
                if (not maybeManifest) {
                    logError("Failed to parse manifest at {}: {}", fileUrl, maybeManifest);
                }

                auto id = maybeManifest.unwrap().id;
                res.put(id, maybeManifest.take());
            }
        }

        return Ok(std::move(res));
    }

    Async::Task<> startAsync(Symbol id);

    Async::Task<> stopAsync(Symbol id);

    Async::Task<> shutdownAsync();
};

} // namespace Strata::Init
