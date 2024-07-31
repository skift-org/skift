#include "objects.h"

namespace Pdf {

void write(Io::Emit &e, Object const &o) {
    o.visit(Visitor{
        [&](None) {
            e("null");
        },
        [&](Ref const &ref) {
            e("{} {} R", ref.num, ref.gen);
        },
        [&](bool b) {
            e(b ? "true" : "false");
        },
        [&](isize i) {
            e("{}", i);
        },
        [&](f64 f) {
            e("{}", f);
        },
        [&](String const &s) {
            e("({})", s);
        },
        [&](Name const &n) {
            e("/{}", n.str());
        },
        [&](Array const &a) {
            e('[');
            for (usize i = 0; i < a.len(); ++i) {
                if (i > 0) {
                    e(' ');
                }
                write(e, a[i]);
            }
            e(']');
        },
        [&](Dict const &d) {
            e("<<\n");
            for (auto const &[k, v] : d.iter()) {
                e('/');
                e(k);
                e(' ');
                write(e, v);
                e('\n');
            }
            e(">>");
        },
        [&](Stream const &s) {
            write(e, Dict{s.dict});
            e("stream\n");
            (void)e.flush();
            (void)e.write(s.data);
            e("endstream\n");
        }
    });
}

void write(Io::Emit &e, File const &f) {
    e("%{}\n", f.header);

    XRef xref;

    for (auto const &[k, v] : f.body.iter()) {
        xref.add(e.total(), k.gen);
        e("{} {} obj\n", k.num, k.gen);
        write(e, v);
        e("\nendobj\n");
    }

    auto startxref = e.total();

    e("xref\n");
    write(e, xref);

    e("trailer\n");
    write(e, f.trailer);

    e("startxref\n");
    e("{}\n", startxref);
    e("%%EOF");
}

void write(Io::Emit &e, XRef const &x) {
    e("0 {}\n", x.entries.len() + 1);
    for (usize i = 0; i < x.entries.len(); ++i) {
        auto const &entry = x.entries[i];
        if (entry.used) {
            e("{:010} {:05} n\n", entry.offset, entry.gen);
        }
    }
}

} // namespace Pdf
