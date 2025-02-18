#pragma once

#include <karm-print/page.h>
#include <karm-print/printer.h>
#include <vaev-dom/document.h>

namespace Vaev::Driver {

Generator<Print::Page> print(Gc::Ref<Dom::Document> dom, Print::Settings const& settings);

} // namespace Vaev::Driver
