#pragma once

#define log(x...) __log(__FUNCTION__, x)
void __log(const char * file, const char *message, ...);