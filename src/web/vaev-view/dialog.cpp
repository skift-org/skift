module;

#include <karm-kira/print-dialog.h>
#include <vaev-dom/document.h>

export module Vaev.View:dialog;

import Vaev.Driver;

namespace Vaev::View {

export Ui::Child printDialog(Gc::Ref<Dom::Document> dom) {
    return Kr::printDialog([dom](Print::Settings const& settings) -> Vec<Print::Page> {
        return Driver::print(dom, settings) | collect<Vec<Print::Page>>();
    });
}

} // namespace Vaev::View
