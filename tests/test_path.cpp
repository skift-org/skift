#include <stdio.h>

#include <libsystem/Assert.h>
#include <libutils/Path.h>

int main(int, char const *[])
{
    auto home_user_local = Path::parse("/home/user/local");

    assert(home_user_local.absolute());
    assert(!home_user_local.relative());

    auto other_home_user_local = Path::parse("/home/user/local");

    assert(home_user_local == other_home_user_local);
    assert(home_user_local.string() == "/home/user/local");

    auto user_local = Path::parse("user/local");

    assert(!user_local.absolute());
    assert(user_local.relative());

    auto home = Path::parse("/home/././././");
    auto join_home_user_local = Path::join(home, user_local);
    join_home_user_local = join_home_user_local.normalized();

    assert(join_home_user_local == home_user_local);
    assert(join_home_user_local.absolute());
    assert(!join_home_user_local.relative());

    return 0;
}
