#pragma once

#include <libutils/Scanner.h>

#include <libhtml/dom/Node.h>

namespace html
{

RefPtr<Node> parse(Scanner &scan);

} // namespace html
