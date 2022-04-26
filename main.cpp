#include <karm-text/str.h>

using namespace Karm::Base;
using namespace Karm::Text;

int main() {
    Str a = u8"test";
    Str b = u8"test";

    return Op::gteq(a, b);
}
