#include <karm-mime/path.h>
#include <karm-test/macros.h>

namespace Karm::Mime::Tests {

test$(pathUpDown) {
    auto path = "/a/b/c/d/e/f"_path;

    auto up1 = path.parent(1);
    expectEq$(up1.str(), "/a/b/c/d/e"_str);

    auto up2 = path.parent(2);
    expectEq$(up2.str(), "/a/b/c/d"_str);

    auto up3 = path.parent(3);
    expectEq$(up3.str(), "/a/b/c"_str);

    auto up4 = path.parent(4);
    expectEq$(up4.str(), "/a/b"_str);

    auto up5 = path.parent(5);
    expectEq$(up5.str(), "/a"_str);

    auto up6 = path.parent(6);
    expectEq$(up6.str(), "/"_str);

    return Ok();
}

} // namespace Karm::Mime::Tests
