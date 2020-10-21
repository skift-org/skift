#pragma once

#include <libsystem/utils/Lexer.h>

#include <libhtml/dom/Node.h>

namespace html
{

RefPtr<Node> parse(Lexer &lexer);

} // namespace html
