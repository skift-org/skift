#include <karm-test/macros.h>
#include <web-select/select.h>

namespace Web::Select::Tests {

test$("select-class-spec") {
    Selector sel = ClassSelector{"foo"s};
    auto el = makeStrong<Dom::Element>(Html::DIV);
    el->classList.add("foo");
    expect$(match(sel, *el));
    return Ok();
}

} // namespace Web::Select::Tests
