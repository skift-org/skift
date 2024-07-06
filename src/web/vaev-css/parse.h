#pragma once

#include <vaev-style/stylesheet.h>

namespace Vaev::Css {

Style::StyleSheet parseStylesheet(Io::SScan &s);

Res<Style::StyleSheet> fetchStylesheet(Mime::Url url);

} // namespace Vaev::Css
