#pragma once

#include <vaev-style/stylesheet.h>

#include "parser.h"

namespace Vaev::Css {

// MARK: Rules -----------------------------------------------------------------

Style::MediaQuery parseMediaQuery(Cursor<Sst> &c);

Res<Style::Prop> parseProperty(Sst const &sst);

Style::Rule parseRule(Sst const &sst);

Style::StyleSheet parseStyleSheet(Sst const &sst);

} // namespace Vaev::Css
