#include <web-base/namespaces.h>

#include "karm-logger/logger.h"
#include "parser.h"

namespace Web::Css {

void Parser::_raise(Str msg) {
    logError("{}: ", msg);
}

void Parser::accept(Token const &t) {
    logDebug("Parsing {} in", t);
}

} // namespace Web::Css
