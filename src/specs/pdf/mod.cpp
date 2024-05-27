#include "mod.h"

namespace Pdf {

// MARK: Header ----------------------------------------------------------------

static Res<Header> parseHeader(Io::SScan &s) {
    if (not s.skip("%PDF-"))
        return Error::invalidData("Expected PDF header");

    Header v;
    v.major = try$(Io::atou(s));
    v.minor = try$(Io::atou(s));

    if (not s.skip("\n%\x81\x81\x81"))
        return Error::invalidData("Expected PDF header");

    return Ok(v);
}

static Res<> emitHeader(Io::Emit &e, Header const &v) {
    e("%%PDF-{}.{}\n%\x81\x81\x81", v.major, v.minor);
    return Ok();
}

// MARK: Document ---------------------------------------------------------------

Res<Document> parse(Io::SScan &s) {
    Document v;
    v.header = try$(parseHeader(s));
    return Ok(v);
}

Res<> emit(Io::Emit &e, Document const &v) {
    try$(emitHeader(e, v.header));

    e("startxref\n");
    e("0\n");
    e("%%EOF\n");
    return Ok();
}

} // namespace Pdf
