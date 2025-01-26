#include <karm-kira/print-dialog.h>
#include <vaev-driver/print.h>

#include "dialog.h"

namespace Vaev::View {

Ui::Child printDialog(Rc<Markup::Document> dom) {
    return Kr::printDialog([dom](Print::Settings const& settings) {
        return Driver::print(*dom, settings);
    });
}

} // namespace Vaev::View
