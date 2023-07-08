#pragma once

namespace Karm::_Embed {

void relaxe();

void enterCritical();

void leaveCritical();

void debug(char const *buf);

[[noreturn]] void panic(char const *buf);

} // namespace Karm::_Embed
