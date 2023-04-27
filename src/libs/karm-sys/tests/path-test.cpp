#include <karm-sys/url.h>
#include <karm-test/macros.h>

test$(pathUpDown) {
    Sys::Path path = "/a/b/c/d/e/f";

    auto up1 = path.up();
    expectEq$(up1.str(), Str{"/a/b/c/d/e"});

    auto up2 = path.up(2);
    expectEq$(up2.str(), Str{"/a/b/c/d"});

    auto up3 = path.up(3);
    expectEq$(up3.str(), Str{"/a/b/c"});

    auto up4 = path.up(4);
    expectEq$(up4.str(), Str{"/a/b"});

    auto up5 = path.up(5);
    expectEq$(up5.str(), Str{"/a"});

    auto up6 = path.up(6);
    expectEq$(up6.str(), Str{"/"});

    return Ok();
}
