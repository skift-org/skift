#pragma once

#include <web-cssom/stylesheet.h>

namespace Web::Css {

Res<CSSOM::StyleSheet> parseStylesheet(Io::SScan &s);

Res<CSSOM::StyleSheet> fetchStylesheet(Mime::Url url);

} // namespace Web::Css
