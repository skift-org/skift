#include "io.h"

namespace Hjert::Core {

Res<Strong<IoNode>> IoNode::create(Hal::PortRange range) {
    return Ok(makeStrong<IoNode>(range));
}

Res<Hj::Arg> IoNode::in(usize offset, usize size) {
    return Ok(Hal::Io::port(_range).readSized(offset, size));
}

Res<> IoNode::out(usize offset, usize size, Hj::Arg arg) {
    Hal::Io::port(_range).writeSized(offset, size, arg);
    return Ok();
}

} // namespace Hjert::Core
