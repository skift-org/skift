#include <karm-test/macros.h>
#include <vaev-style/select.h>

namespace Vaev::Style::Tests {

test$("select-class-spec") {
    Selector sel = ClassSelector{"foo"s};
    auto el = makeStrong<Dom::Element>(Html::DIV);
    el->classList.add("foo");
    expect$(match(sel, *el));
    return Ok();
}

} // namespace Vaev::Style::Tests
