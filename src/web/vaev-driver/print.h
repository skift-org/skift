#pragma once

#include <karm-print/page.h>
#include <karm-print/printer.h>
#include <vaev-markup/dom.h>

namespace Vaev::Driver {

Vec<Print::Page> print(Markup::Document const &dom, Print::Settings const &settings);

} // namespace Vaev::Driver
