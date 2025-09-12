module Vaev.Engine;

import Karm.Core;
import :style.selector;

namespace Vaev::Style {

bool Pseudo::operator==(Pseudo const&) const = default;

bool Infix::operator==(Infix const& other) const = default;

bool Nfix::operator==(Nfix const& other) const = default;

bool Selector::operator==(Selector const&) const = default;

} // namespace Vaev::Style
