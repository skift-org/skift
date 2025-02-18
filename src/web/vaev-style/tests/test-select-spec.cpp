#include <karm-test/macros.h>
#include <vaev-style/select.h>
#include <vaev-style/selector-matching.h>

namespace Vaev::Style::Tests {

test$("select-class-spec") {
    Selector sel = ClassSelector{"foo"s};
    auto el = makeRc<Markup::Element>(Html::DIV);
    el->classList.add("foo");
    expect$(matchSelector(sel, *el));
    return Ok();
}

} // namespace Vaev::Style::Tests
