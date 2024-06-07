#include "sst.h"

namespace Web::Css {

void Sst::repr(Io::Emit &e) const {
    if (type == TOKEN) {
        e("{}\n", token);
        return;
    }

    e("({} ", Io::toParamCase(toStr(type)));
    if (token)
        e("token={}", token);
    e.indent();

    if (prefix) {
        e.newline();
        e("prefix=[");
        e.indentNewline();
        (*prefix)->repr(e);
        e.deindent();
        e("]");
    }

    if (content) {
        e.newline();
        e("content=[");
        e.indentNewline();
        for (auto &child : content) {
            child.repr(e);
        }
        e.deindent();
        e("]");
        e.newline();
    }
    e.deindent();
    e(")\n");
}

Str toStr(Sst::_Type type) {
    switch (type) {
#define ITER(NAME)  \
    case Sst::NAME: \
        return #NAME;
        FOREACH_SST(ITER)
#undef ITER
    default:
        panic("invalid ast type");
    }
}

} // namespace Web::Css
