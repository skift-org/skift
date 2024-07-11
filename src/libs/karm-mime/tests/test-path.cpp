#include <karm-mime/path.h>
#include <karm-test/macros.h>

namespace Karm::Mime::Tests {

test$("karm-mime-path-up-down") {
    auto path = "/a/b/c/d/e/f"_path;

    auto up1 = path.parent(1);
    expectEq$(up1.str(), "/a/b/c/d/e"s);

    auto up2 = path.parent(2);
    expectEq$(up2.str(), "/a/b/c/d"s);

    auto up3 = path.parent(3);
    expectEq$(up3.str(), "/a/b/c"s);

    auto up4 = path.parent(4);
    expectEq$(up4.str(), "/a/b"s);

    auto up5 = path.parent(5);
    expectEq$(up5.str(), "/a"s);

    auto up6 = path.parent(6);
    expectEq$(up6.str(), "/"s);

    return Ok();
}

test$("karm-mime-path-parent-of") {
    expect$(""_path.isParentOf(""_path));
    expect$("/a"_path.isParentOf("/a"_path));
    expect$("/a"_path.isParentOf("/a/b"_path));
    expect$("/a"_path.isParentOf("/a/b/c"_path));
    expect$("/a/b"_path.isParentOf("/a/b/c"_path));
    expectNot$("/a/c"_path.isParentOf("/a/b/c"_path));
    expect$("."_path.isParentOf("."_path));

    return Ok();
}

test$("karm-mime-path-str") {
    expectEq$(""_path.str(), ".");
    expectEq$("/a/b/c"_path.str(), "/a/b/c");
    expectEq$("a/b/c"_path.str(), "a/b/c");
    expectEq$("a/b/c/"_path.str(), "a/b/c");
    expectEq$("a/b/c/."_path.str(), "a/b/c/.");
    expectEq$("a/b/c/.."_path.str(), "a/b/c/..");
    expectEq$("a/b/c/../"_path.str(), "a/b/c/..");

    return Ok();
}

} // namespace Karm::Mime::Tests
