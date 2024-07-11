#include <karm-test/macros.h>
#include <karm-text/book.h>

namespace Karm::Text::Tests {

test$("karm-text-common-family") {
    expectEq$(commonFamily("Noto", "Noto"), "Noto");
    expectEq$(commonFamily("Not", "Noto"), "");
    expectEq$(commonFamily("Noto", "Arial"), "");
    expectEq$(commonFamily("Noto Sans Condensed", "Noto Sans Condensed Bold"), "Noto Sans Condensed");
    expectEq$(commonFamily("Noto Sans ExtraCondensed", "Noto Sans Condensed Bold"), "Noto Sans");
    expectEq$(commonFamily("Comic Sans", "Comic Serif"), "Comic");

    return Ok();
}

} // namespace Karm::Text::Tests
