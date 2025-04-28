module;

#include <karm-print/page.h>
#include <vaev-dom/document.h>

export module Vaev.View:dialog;

import Vaev.Driver;
import Karm.Kira;
import Karm.Ui;
import Hideo.Printers;

namespace Vaev::View {

export Ui::Child printDialog(Gc::Ref<Dom::Document> dom) {
    return Hideo::Printers::printDialog([dom](Print::Settings const& settings) -> Vec<Print::Page> {
        return Driver::print(dom, settings) | collect<Vec<Print::Page>>();
    });
}

} // namespace Vaev::View
