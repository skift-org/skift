#include <karm-sys/chan.h>

#include "driver.h"
#include "test.h"

namespace Karm::Test {

void Driver::add(Test *test) {
    _tests.pushBack(test);
}

void Driver::runAll() {
    for (auto *test : _tests) {
        auto result = test->run(*this);
        if (!result) {
            Sys::errln("Test {} Failled", test->_name);
        }
    }
}

bool Driver::beginDescribe(Str name, Loc) {
    Sys::errln("Describe {}", name);
    return true;
}

void Driver::endDescribe() {}

bool Driver::beginIt(Str name, Loc) {
    Sys::errln("\tIt {}", name);
    return true;
}

void Driver::endIt() {}

Driver &driver() {
    static Driver driver;
    return driver;
}

} // namespace Karm::Test
