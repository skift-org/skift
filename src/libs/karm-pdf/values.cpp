#include "values.h"

namespace Karm::Pdf {

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
        [&](Name const& n) {
            e("/{}", n.str());
        },
        [&](Array const& a) {
            e('[');
            for (usize i = 0; i < a.len(); ++i) {
                if (i > 0) {
                    e(' ');
                }
                a[i].write(e);
            }
            e(']');
        },
        [&](Dict const& d) {
            e("<<\n");
            for (auto const& [k, v] : d.iter()) {
                e('/');
                e(k);
                e(' ');
                v.write(e);
                e('\n');
            }
            e(">>");
        },
        [&](Stream const& s) {
            Value{s.dict}.write(e);
            e("stream\n");
            (void)e.flush();
            (void)e.write(s.data);
            e("\nendstream\n");
        }
    });
}

void File::write(Io::Emit& e) const {
    e("%{}\n", header);
    e("%Powered By Karm PDF 🐢🏳️‍⚧️🦔\n", header);

    XRef xref;

    for (auto const& [k, v] : body.iter()) {
        xref.add(e.total(), k.gen);
        e("{} {} obj\n", k.num, k.gen);
        v.write(e);
        e("\nendobj\n");
    }

    (void)e.flush();
    auto startxref = e.total();
    e("xref\n");
    xref.write(e);

    e("trailer\n");
    Value{trailer}.write(e);

    e("startxref\n");
    e("{}\n", startxref);
    e("%%EOF");
}

void XRef::write(Io::Emit& e) const {
    e("0 {}\n", entries.len() + 1);
    for (usize i = 0; i < entries.len(); ++i) {
        auto const& entry = entries[i];
        if (entry.used) {
            e("{:010} {:05} n\n", entry.offset, entry.gen);
        }
    }
}

} // namespace Karm::Pdf
