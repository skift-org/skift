#include <karm-test/macros.h>
#include <web-xml/parser.h>

namespace Web::Xml::Tests {

test$("parse empty document") {
    auto s = Io::SScan(""s);
    auto p = Parser();
    expect$(not p.parse(s, Web::HTML));
    return Ok();
}

} // namespace Web::Xml::Tests
