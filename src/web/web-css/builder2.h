#pragma once

#include <web-style/stylesheet.h>

#include "parser.h"

namespace Web::Css {

// MARK: Types -----------------------------------------------------------------

Res<Color> parseColor(Cursor<Sst> &c);

Res<Length> parseLength(Cursor<Sst> &c);

Res<Size> parseSize(Cursor<Sst> &c);

Res<Percent> parsePercentage(Cursor<Sst> &c);

Res<PercentOr<Length>> parseLengthOrPercentage(Cursor<Sst> &c);

// MARK: Rules -----------------------------------------------------------------

Style::MediaQuery parseMediaQuery(Cursor<Sst> &);

Style::Selector parseSelector(Cursor<Sst> &);

Res<Style::Prop> parseProperty(Sst const &sst);

Style::Rule parseRule(Sst const &sst);

Style::StyleSheet parseStyleSheet(Sst const &sst);

} // namespace Web::Css
