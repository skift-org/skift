#include <karm-sys/entry.h>

import Vaev.Script;
import Karm.Core;
import Karm.Gc;

using namespace Karm;
using namespace Vaev;

Async::Task<> entryPointAsync(Sys::Context&) {
    Gc::Heap heap;

    auto agent = heap.alloc<Script::Agent>(heap);
    auto realm = heap.alloc<Script::Realm>(agent);

    (void)realm->initializeHostDefinedRealm(agent);

    auto object1 = Script::Object::create(*agent);
    (void)object1->defineOwnProperty(
        Script::PropertyKey::from(u"foo"_s16),
        {
            .value = Script::Number{42.},
        }
    );

    auto object2 = Script::Object::create(
        *agent,
        {
            .prototype = object1,
        }
    );

    auto res = object2->get(Script::PropertyKey::from(u"foo"_s16), object2);

    Sys::print("object2.foo = {}\n", res);

    co_return Ok();
}
