#pragma once

#include <web-style/stylesheet.h>

#include "parser.h"

namespace Web::Css {

// No spec, we take the SST we built and convert it to a usable list of rules
Vec<Style::Rule> parseSST(Sst);

} // namespace Web::Css
