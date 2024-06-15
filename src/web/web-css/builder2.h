#pragma once

#include <web-style/stylesheet.h>

#include "parser.h"

namespace Web::Css {

Style::MediaQuery parseMediaQuery(Cursor<Sst> &);

Style::Selector parseSelector(Cursor<Sst> &);

Res<Style::Prop> parseProperty(Sst const &sst);

Style::Rule parseRule(Sst const &sst);

Style::StyleSheet parseStyleSheet(Sst const &sst);

} // namespace Web::Css
