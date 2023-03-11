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
        if (not result) {
            Sys::errln("Test {} Failled", test->_name);
        }
    }
}

Driver &driver() {
    static Driver driver;
    return driver;
}

} // namespace Karm::Test
