#include <karm-io/pack.h>
#include <karm-test/macros.h>

namespace Karm::Io::Tests {

test$("pack-unpack") {
    auto testCase = [&]<typename T>(T const& input) -> Res<> {
        Io::BufferWriter buf;
        Io::PackEmit e{buf};

        try$(Io::pack(e, input));

        Io::PackScan s{buf.bytes(), {}};
        auto output = try$(Io::unpack<T>(s));
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

} // namespace Karm::Io::Tests
