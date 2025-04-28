#include <karm-test/macros.h>
#include <karm-text/edit.h>

namespace Karm::Text::Tests {

test$("karm-text-model-moves") {
    Model mdl{"foo bar baz"};

    mdl.moveStart();
    expectEq$(mdl._cur.head, 0uz);

    mdl.moveNext();
    expectEq$(mdl._cur.head, 1uz);

    mdl.movePrev();
    expectEq$(mdl._cur.head, 0uz);

    mdl.moveEnd();
    expectEq$(mdl._cur.head, 11uz);

    mdl.moveStart();
    expectEq$(mdl._cur.head, 0uz);

    return Ok();
}

} // namespace Karm::Text::Tests
