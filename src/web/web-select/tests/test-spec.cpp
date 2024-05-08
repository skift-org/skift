#include <karm-test/macros.h>
#include <web-select/select.h>

namespace Web::Select::Tests {

test$("select-class-spec") {
    Selector sel = ClassSelector{"foo"s};
    auto el = makeStrong<Dom::Element>();
    el->classList.add("foo");
    auto spec = match(sel, *el);
    expectEq$(spec, Spec::C);
    return Ok();
}

} // namespace Web::Select::Tests
