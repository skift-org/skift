#pragma once

#include <karm-base/box.h>
#include <karm-base/vec.h>
#include <vaev-css/parser.h>
#include <vaev-markup/dom.h>
#include <vaev-markup/tags.h>

#include "select.h"

namespace Vaev::Style {

bool matchSelector(Selector const& selector, Markup::Element const& el);

Opt<Spec> matchSelectorWithSpecificity(Selector const& selector, Markup::Element const& el);

} // namespace Vaev::Style
