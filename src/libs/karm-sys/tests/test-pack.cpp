#include <karm-test/macros.h>
import Karm.Core;

namespace Karm::Sys::Tests {

test$("pack-unpack") {
    auto testCase = [&]<typename T>(T const& input) -> Res<> {
        Io::BufferWriter buf;
        MessageWriter e{buf};

        try$(Sys::pack(e, input));

        MessageReader s{buf.bytes(), {}};
        auto output = try$(Sys::unpack<T>(s));
        expectEq$(input, output);
        return Ok();
    };

    try$(testCase(42));
    try$(testCase(0));
    try$(testCase(-1));
    try$(testCase(String{"Hello, world"}));
    try$(testCase(String{"Hello,\0 world"}));

    return Ok();
}

} // namespace Karm::Sys::Tests
