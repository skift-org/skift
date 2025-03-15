#pragma once

#include "calc.h"
#include "keywords.h"
#include "length.h"
#include "percent.h"

namespace Vaev {

using Width = FlatUnion<Keywords::Auto, CalcValue<PercentOr<Length>>>;

} // namespace Vaev
