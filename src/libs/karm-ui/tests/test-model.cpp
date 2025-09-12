#include <karm-test/macros.h>

import Karm.Ui;

namespace Karm::Ui::Tests {

test$("karm-ui-model-moves") {
    TextModel mdl{"foo bar baz"};

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

} // namespace Karm::Ui::Tests
