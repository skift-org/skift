#pragma once

namespace Embed {

void relaxe();

void enterCritical();

void leaveCritical();

void debug(char const *buf);

[[noreturn]] void panic(char const *buf);

} // namespace Embed
