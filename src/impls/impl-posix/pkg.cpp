#include <stdlib.h>

//
#include <karm-sys/_embed.h>
#include <karm-sys/proc.h>

#include "utils.h"

namespace Karm::Sys::_Embed {

Res<Vec<String>> installedBundles() {
    auto [repo, format] = try$(Posix::repoRoot());
    Ref::Url repoRoot;
    if (format == Posix::RepoType::CUTEKIT) {
        repoRoot = Ref::parseUrlOrPath(repo, try$(Sys::pwd()));
    } else if (format == Posix::RepoType::PREFIX) {
        repoRoot = Ref::parseUrlOrPath(repo, try$(Sys::pwd()))
                       .join("share");
    } else {
        return Error::notFound("unknown repo type");
    }

    auto dirs = try$(Sys::_Embed::readDir(repoRoot));
    Vec<String> ids;
    for (auto& dir : dirs) {
        if (dir.type == Sys::Type::DIR)
            ids.pushBack(dir.name);
    }
    return Ok(ids);
}

Res<String> currentBundle() {
    auto* maybeBundle = getenv("CK_COMPONENT");
    if (maybeBundle)
        return Ok(Str::fromNullterminated(maybeBundle));

    maybeBundle = getenv("SKIFT_COMPONENT");
    if (maybeBundle)
        return Ok(Str::fromNullterminated(maybeBundle));

    return Error::notFound("no bundle found");
}

} // namespace Karm::Sys::_Embed
