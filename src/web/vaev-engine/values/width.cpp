export module Vaev.Engine:values.width;

import Karm.Core;

import :values.calc;
import :values.keywords;
import :values.length;
import :values.percent;

using namespace Karm;

namespace Vaev {

export using Width = FlatUnion<
    Keywords::Auto,
    CalcValue<PercentOr<Length>>>;

} // namespace Vaev
