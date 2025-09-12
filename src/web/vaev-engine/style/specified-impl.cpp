module Vaev.Engine;

import :style.props;

namespace Vaev::Style {

SpecifiedValues const& SpecifiedValues::initial() {
    static SpecifiedValues computed = [] {
        SpecifiedValues res{};
        StyleProp::any([&]<typename T>() {
            if constexpr (requires { T::initial(); })
                T{}.apply(res);
        });
        return res;
    }();
    return computed;
}

} // namespace Vaev::Style
