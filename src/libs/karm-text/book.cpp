#include <karm-logger/logger.h>
#include <karm-pkg/bundle.h>
#include <karm-sys/time.h>

#include "book.h"
#include "loader.h"

namespace Karm::Text {

Res<FontBook> loadAll() {
    FontBook book;

    auto start = Sys::now();

    auto bundles = try$(Pkg::installedBundles());
    for (auto &bundle : bundles) {
        auto maybeDir = Sys::Dir::open(bundle.url() / "fonts");
        if (not maybeDir)
            continue;

        auto dir = maybeDir.take();

        for (auto &diren : dir.entries()) {
            if (diren.isDir)
                continue;

            auto fontUrl = dir.path() / diren.name;

            auto maybeFont = loadFontface(fontUrl);
            if (not maybeFont)
                continue;

            auto font = maybeFont.take();

            book.add({
                .url = fontUrl,
                .attrs = font->attrs(),
            });
        }
    }

    auto elapsed = Sys::now() - start;
    logDebug("Loaded {} fonts in {}", book._faces.len(), elapsed);

    return Ok(book);
}

} // namespace Karm::Text
