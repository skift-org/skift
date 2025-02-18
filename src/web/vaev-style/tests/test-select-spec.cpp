#include <karm-gc/heap.h>
#include <karm-test/macros.h>
#include <vaev-style/matcher.h>
#include <vaev-style/selector.h>

namespace Vaev::Style::Tests {

test$("select-class-spec") {
    Gc::Heap gc;
    Selector sel = ClassSelector{"foo"s};
    auto el = gc.alloc<Dom::Element>(Html::DIV);
    el->classList.add("foo");
    expectNe$(matchSelector(sel, el), NONE);
    return Ok();
}

} // namespace Vaev::Style::Tests
