#include <libsystem/io_new/Streams.h>

int main(int argc, char const *argv[])
{
    __unused(argc);
    __unused(argv);

    System::outln("number: {}", 1234.1234);
    System::outln("number: {}", 1234.1234f);
    System::outln("number: {}", 12341234);

    System::outln("What your name?");

    String name = System::inln().value();

    System::outln("How old are you?");

    String age = System::inln().value();

    System::outln("Hello {}! Your are {} years old!", name, age);

    return 0;
}
