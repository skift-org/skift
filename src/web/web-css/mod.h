#pragma once

#include <web-style/stylesheet.h>

namespace Web::Css {

Style::StyleSheet parseStylesheet(Io::SScan &s);

Res<Style::StyleSheet> fetchStylesheet(Mime::Url url);

} // namespace Web::Css
