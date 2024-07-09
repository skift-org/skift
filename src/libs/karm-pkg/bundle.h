#pragma once

#include <karm-mime/url.h>

namespace Karm::Pkg {

struct BundleInfo {
    String id;

    Mime::Url url() {
        Mime::Url url;
        url.scheme = "bundle"s;
        url.host = id;
        return url;
    }
};

Res<BundleInfo> currentBundle();

Res<Vec<BundleInfo>> installedBundles();

} // namespace Karm::Pkg
