#include <karm-mime/path.h>
#include <karm-test/macros.h>

test$(pathUpDown) {
    auto path = "/a/b/c/d/e/f"_path;

    auto up1 = path.parent(1);
    expectEq$(up1.str(), Str{"/a/b/c/d/e"});

    auto up2 = path.parent(2);
    expectEq$(up2.str(), Str{"/a/b/c/d"});

    auto up3 = path.parent(3);
    expectEq$(up3.str(), Str{"/a/b/c"});

    auto up4 = path.parent(4);
    expectEq$(up4.str(), Str{"/a/b"});

    auto up5 = path.parent(5);
    expectEq$(up5.str(), Str{"/a"});

    auto up6 = path.parent(6);
    expectEq$(up6.str(), Str{"/"});

    return Ok();
}
