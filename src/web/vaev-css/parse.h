#pragma once

#include <vaev-style/stylesheet.h>

namespace Vaev::Css {

Style::StyleSheet parseStylesheet(Io::SScan &s);

Res<Style::StyleSheet> fetchStylesheet(Mime::Url url);

Style::Selector parseSelector(Io::SScan &s);

Style::Selector parseSelector(Str input);

} // namespace Vaev::Css
