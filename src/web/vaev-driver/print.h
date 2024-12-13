#pragma once

#include <karm-print/printer.h>
#include <karm-scene/page.h>
#include <vaev-markup/dom.h>

namespace Vaev::Driver {

Vec<Strong<Scene::Page>> print(Markup::Document const &dom, Print::Settings const &settings);

} // namespace Vaev::Driver
