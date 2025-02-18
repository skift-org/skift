#include <karm-io/aton.h>
#include <karm-math/funcs.h>
#include <karm-test/macros.h>

namespace Karm::Io::Tests {

test$("readline-ends-with-delim-len-1") {

    BufReader bufReader{bytes("hello\nworldd\n"s)};

    BufferWriter bufWriter;

    {
        auto [read, untilDel] = try$(
            readLine(bufReader, bufWriter, bytes("\n"s))
        );
        expectEq$(bufWriter.take(), bytes("hello\n"s));
        expectEq$(read, 5u);
        expect$(untilDel);
    }

    {
        auto [read, untilDel] = try$(readLine(
            bufReader, bufWriter, bytes("\n"s)
        ));
        expectEq$(bufWriter.take(), bytes("worldd\n"s));
        expectEq$(read, 6u);
        expect$(untilDel);
    }

    return Ok();
}

test$("readline-ends-with-stream") {

    BufReader bufReader{bytes("hello\nwrld"s)};

    BufferWriter bufWriter;

    {
        auto [read, untilDel] = try$(readLine(
            bufReader, bufWriter, bytes("\n"s)
        ));
        expectEq$(bufWriter.take(), bytes("hello\n"s));

        expectEq$(read, 5u);
        expect$(untilDel);
    }

    {
        auto [read, untilDel] = try$(readLine(
            bufReader, bufWriter, bytes("\n"s)
        ));
        expectEq$(bufWriter.take(), bytes("wrld"s));
        expectEq$(read, 4u);
        expectNot$(untilDel);
    }

    return Ok();
}

test$("readline-ends-with-delim-len-5") {

    BufReader bufReader{bytes("hello12345worlds12345he12345"s)};

    BufferWriter bufWriter;

    {
        auto [read, untilDel] = try$(readLine(
            bufReader, bufWriter, bytes("12345"s)
        ));
        expectEq$(bufWriter.take(), bytes("hello12345"s));
        expectEq$(read, 5u);
        expect$(untilDel);
    }

    {
        auto [read, untilDel] = try$(readLine(
            bufReader, bufWriter, bytes("12345"s)
        ));
        expectEq$(bufWriter.take(), bytes("worlds12345"s));
        expectEq$(read, 6u);
        expect$(untilDel);
    }

    {
        auto [read, untilDel] = try$(readLine(
            bufReader, bufWriter, bytes("12345"s)
        ));
        expectEq$(bufWriter.take(), bytes("he12345"s));
        expectEq$(read, 2u);
        expect$(untilDel);
    }

    return Ok();
}

} // namespace Karm::Io::Tests
