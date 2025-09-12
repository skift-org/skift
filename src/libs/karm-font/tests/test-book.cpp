module;

#include <karm-test/macros.h>

module Karm.Font;

import :database;

namespace Karm::Font::Tests {

test$("karm-font-common-family") {
    expectEq$(_commonFamily("Noto"_sym, "Noto"_sym), "Noto"_sym);
    expectEq$(_commonFamily("Not"_sym, "Noto"_sym), ""_sym);
    expectEq$(_commonFamily("Noto"_sym, "Arial"_sym), ""_sym);
    expectEq$(_commonFamily("Noto Sans Condensed"_sym, "Noto Sans Condensed Bold"_sym), "Noto Sans Condensed"_sym);
    expectEq$(_commonFamily("Noto Sans ExtraCondensed"_sym, "Noto Sans Condensed Bold"_sym), "Noto Sans"_sym);
    expectEq$(_commonFamily("Comic Sans"_sym, "Comic Serif"_sym), "Comic"_sym);

    return Ok();
}

} // namespace Karm::Font::Tests
