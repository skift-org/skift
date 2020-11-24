#include <stdio.h>

#include <libsystem/Assert.h>
#include <libutils/String.h>

int main(int, char const *[])
{
    String hello_world = "Hello, world!";

    assert(hello_world == "Hello, world!");
    assert(hello_world != "Something else blablabla!");
    assert(!hello_world.null_or_empty());

    String empty = "";

    assert(empty == "");
    assert(empty != "Something else blablabla!");
    assert(empty.null_or_empty());

    String first = "first";
    String second = "second";

    first = move(second);

    assert(first == "second");
    assert(second == "first");

    return 0;
}
