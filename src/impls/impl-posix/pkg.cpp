#include <karm-pkg/_embed.h>
#include <karm-sys/_embed.h>

#include "utils.h"

namespace Karm::Pkg::_Embed {

Res<Vec<String>> installedBundles() {
    auto repoRoot = try$(Mime::parseUrlOrPath(try$(Posix::repoRoot())));
    auto dirs = try$(Sys::_Embed::readDir(repoRoot));
    Vec<String> ids;
    for (auto &dir : dirs) {
        if (dir.type == Sys::Type::DIR)
            ids.pushBack(dir.name);
    }
    return Ok(ids);
}

Res<String> currentBundle() {
    auto *maybeBundle = getenv("CK_COMPONENT");
    if (maybeBundle)
        return Ok(Str::fromNullterminated(maybeBundle));

    maybeBundle = getenv("SKIFT_COMPONENT");
    if (maybeBundle)
        return Ok(Str::fromNullterminated(maybeBundle));

    return Error::notFound("no bundle found");
}

} // namespace Karm::Pkg::_Embed
