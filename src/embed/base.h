#pragma once

namespace Embed {

void relaxe();

void criticalEnter();

void criticalLeave();

void debug(char const *buf);

[[noreturn]] void panic(char const *buf);

} // namespace Embed
