module;

#include <karm-sys/file.h>

export module Karm.Fs:image;

import :node;

namespace Karm::Fs {

export struct Image : Node {
    Sys::File _file;

    Image(Sys::File file) : _file(std::move(file)) {}

    static Async::Task<Rc<Node>> openOrCreateAsync(Ref::Url const& url) {
        auto file = co_try$(Sys::File::openOrCreate(url));
        co_return co_trya$(Fs::createAsync<Image>(std::move(file)));
    }
};

} // namespace Karm::Fs
