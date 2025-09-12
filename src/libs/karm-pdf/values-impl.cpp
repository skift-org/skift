module;

#include <karm-core/macros.h>

module Karm.Pdf;

import Karm.Core;

namespace Karm::Pdf {

void Name::write(Io::Emit& e) const {
    e("/{}", str());
}

void Array::write(Io::Emit& e) const {
    e('[');
    for (usize i = 0; i < len(); ++i) {
        if (i > 0) {
            e(' ');
        }
        buf()[i].write(e);
    }
    e(']');
}

void Dict::write(Io::Emit& e) const {
    e("<<\n");
    for (auto const& [k, v] : iter()) {
        e('/');
        e(k);
        e(' ');
        v.write(e);
        e('\n');
    }
    e(">>");
}

void Stream::write(Io::Emit& e) const {
    dict.write(e);
    e("stream\n");
    (void)e.flush();
    (void)e.write(data);
    e("\nendstream\n");
}

void Value::write(Io::Emit& e) const {
    visit(Visitor{
        [&](None) {
            e("null");
        },
        [&](Ref const& ref) {
            e("{} {} R", ref.num, ref.gen);
        },
        [&](bool b) {
            e(b ? "true" : "false");
        },
        [&](isize i) {
            e("{}", i);
        },
        [&](usize i) {
            e("{}", i);
        },
        [&](f64 f) {
            e("{}", f);
        },
        [&](String const& s) {
            e("({})", s);
        },
        [&](auto const& v) {
            v.write(e);
        },
    });
}

Res<> File::write(Io::Writer& w) const {
    Io::Count count{w};
    Io::TextEncoder<> enc{count};
    Io::Emit e{enc};
    e("%{}\n", header);
    e("%Powered By Karm PDF 🐢🏳️‍⚧️🦔\n", header);

    XRef xref;

    for (auto const& [k, v] : body.iter()) {
        try$(e.flush());
        xref.add(try$(Io::tell(count)), k.gen);
        e("{} {} obj\n", k.num, k.gen);
        v.write(e);
        e("\nendobj\n");
    }

    try$(e.flush());
    auto startxref = try$(Io::tell(count));
    e("xref\n");
    xref.write(e);

    e("trailer\n");
    trailer.write(e);

    e("\nstartxref\n");
    e("{}\n", startxref);
    e("%%EOF");

    return Ok();
}

void XRef::write(Io::Emit& e) const {
    e("1 {}\n", entries.len());
    for (usize i = 0; i < entries.len(); ++i) {
        auto const& entry = entries[i];
        if (entry.used) {
            e("{:010} {:05} n\n", entry.offset, entry.gen);
        }
    }
}

} // namespace Karm::Pdf
