#include <karm-test/macros.h>
#include <textbox/model.h>

namespace Textbox {

test$(modelMoves) {
    Model mdl{"foo bar baz"};

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

} // namespace Textbox
