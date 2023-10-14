#include <karm-test/macros.h>
#include <url/url.h>

test$(pathUpDown) {
    auto path = Url::Path::parse("/a/b/c/d/e/f");

    auto up1 = path.parent(1);
    expectEq$(try$(up1.str()), Str{"/a/b/c/d/e"});

    auto up2 = path.parent(2);
    expectEq$(try$(up2.str()), Str{"/a/b/c/d"});

    auto up3 = path.parent(3);
    expectEq$(try$(up3.str()), Str{"/a/b/c"});

    auto up4 = path.parent(4);
    expectEq$(try$(up4.str()), Str{"/a/b"});

    auto up5 = path.parent(5);
    expectEq$(try$(up5.str()), Str{"/a"});

    auto up6 = path.parent(6);
    expectEq$(try$(up6.str()), Str{"/"});

    return Ok();
}
