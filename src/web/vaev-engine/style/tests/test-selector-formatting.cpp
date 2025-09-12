#include <karm-test/macros.h>

import Vaev.Engine;

using namespace Karm;

namespace Vaev::Style::Tests {

test$("selector-formatting") {

    auto sel = try$(Selector::parse(".a"));
    expectEq$(
        ".a"s,
        Io::toStr(sel.unparsed())
    );

    sel = try$(Selector::parse(".a .b"));
    expectEq$(
        ".a .b"s,
        Io::toStr(sel.unparsed())
    );

    sel = try$(Selector::parse(".a.b"));
    expectEq$(
        ".a.b"s,
        Io::toStr(sel.unparsed())
    );

    sel = try$(Selector::parse(".a,.b"));
    expectEq$(
        ".a,.b"s,
        Io::toStr(sel.unparsed())
    );

    sel = try$(Selector::parse(".a>.b"));
    expectEq$(
        ".a>.b"s,
        Io::toStr(sel.unparsed())
    );

    sel = try$(Selector::parse(".a~.b"));
    expectEq$(
        ".a~.b"s,
        Io::toStr(sel.unparsed())
    );

    sel = try$(Selector::parse(".a+.b"));
    expectEq$(
        ".a+.b"s,
        Io::toStr(sel.unparsed())
    );

    return Ok();
}

} // namespace Vaev::Style::Tests
