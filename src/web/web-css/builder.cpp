#include "builder.h"
#include <karm-logger/logger.h>

namespace Web::Css {
// No spec, we take the SST we built and convert it to a usable list of rules
static inline Vec<Style::Rule> parseSST(Sst);

} // namespace Web::Css
