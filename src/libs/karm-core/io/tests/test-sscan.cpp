#include <karm-test/macros.h>
import Karm.Core;

namespace Karm::Io::Tests {

test$("sscan-ended") {
    SScan s{""s};
    expect$(s.ended());

    s = SScan{"a"s};
    expect$(not s.ended());

    return Ok();
}

test$("sscan-rem") {
    SScan s{"abc"};
    expect$(s.rem() == 3);

    s = SScan{"abc"};
    s.next();
    expect$(s.rem() == 2);

    s = SScan{"abc"};
    s.next(3);
    expect$(s.rem() == 0);

    return Ok();
}

test$("sscan-rem-str") {
    SScan s{"abc"};
    expect$(s.remStr() == "abc");

    s = SScan{"abc"};
    s.next();
    expect$(s.remStr() == "bc");

    s = SScan{"abc"};
    s.next(3);
    expect$(s.remStr() == "");

    return Ok();
}

test$("sscan-curr") {
    SScan s{"abc"};
    expect$(s.peek() == 'a');

    s = SScan{"abc"};
    s.next();
    expect$(s.peek() == 'b');

    s = SScan{"abc"};
    s.next(3);
    expect$(s.peek() == '\0');

    return Ok();
}

test$("sscan-peek") {
    SScan s{"abc"};

    expect$(s.peek() == 'a');
    expect$(s.peek(1) == 'b');
    expect$(s.peek(2) == 'c');
    expect$(s.peek(3) == '\0');
    expect$(s.peek(4) == '\0');

    return Ok();
}

test$("sscan-next") {
    SScan s{"abc"};

    expect$(s.next() == 'a');
    expect$(s.next() == 'b');
    expect$(s.next() == 'c');
    expect$(s.next() == '\0');
    expect$(s.next() == '\0');

    return Ok();
}

test$("sscan-skip") {
    SScan s{"abc"};

    expect$(s.skip('a'));
    expect$(s.rem() == 2);
    expect$(s.skip('b'));
    expect$(s.rem() == 1);
    expect$(s.skip('c'));
    expect$(s.rem() == 0);

    expect$(not s.skip('d'));
    expect$(s.rem() == 0);

    s = SScan{"abc"};
    expect$(s.skip("ab"));
    expect$(s.rem() == 1);
    expect$(s.skip("c"));
    expect$(s.rem() == 0);
    expect$(not s.skip("d"));
    expect$(s.rem() == 0);

    return Ok();
}

test$("sscan-eat") {
    SScan s{"abc"};

    expect$(s.eat('a'));
    expect$(s.eat('b'));
    expect$(s.eat('c'));
    expect$(not s.eat('d'));

    s = SScan{"abc"};
    expect$(s.eat("ab"));
    expect$(s.eat("c"));
    expect$(not s.eat("d"));

    s = SScan{"aaaaaa"};
    expect$(s.eat('a'));
    expect$(s.ended());

    return Ok();
}

test$("sscan-ahead") {
    SScan s{"abc"};

    expect$(s.ahead('a'));
    expectNot$(s.ahead('b'));
    expect$(s.rem() == 3);

    expect$(s.ahead("ab"));
    expectNot$(s.ahead("bc"));
    expect$(s.rem() == 3);

    return Ok();
}

} // namespace Karm::Io::Tests
