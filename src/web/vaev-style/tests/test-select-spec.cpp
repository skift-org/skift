#include <karm-test/macros.h>
#include <vaev-style/select.h>

namespace Vaev::Style::Tests {

test$("select-class-spec") {
    Selector sel = ClassSelector{"foo"s};
    auto el = makeStrong<Markup::Element>(Html::DIV);
    el->classList.add("foo");
    expect$(sel.match(*el));
    return Ok();
}

} // namespace Vaev::Style::Tests
