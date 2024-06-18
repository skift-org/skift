#include "objects.h"

namespace Pdf {

void write(Io::Emit &e, Object const &o) {
    o.visit(Visitor{
        [&](None) {
            e("null");
        },
        [&](Ref const &ref) {
            e(ref.num);
            e(' ');
            e(ref.gen);
            e(" R");
        },
        [&](bool b) {
            e(b ? "true" : "false");
        },
        [&](isize i) {
            e(i);
        },
        [&](f64 f) {
            e(f);
        },
        [&](String const &s) {
            e('(');
            e(s);
            e(')');
        },
        [&](Name const &n) {
            e('/');
            e(n.value);
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
                e(k.value);
                e(' ');
                write(e, v);
                e('\n');
            }
            e(">>");
        },
        [&](Stream const &s) {
            write(e, Dict{s.dict});
            e("\nstream\n");
            (void)e.write(s.data);
            e("\nendstream");
        }
    });
}

} // namespace Pdf
