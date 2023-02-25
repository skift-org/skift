#include <stdint.h>
#include <string.h>

int strEq(const char *a, size_t aLen, const char *b) {
    return aLen == strlen(b) && memcmp(a, b, aLen) == 0;
}

uint32_t _Mdi__byName(char const *query, size_t queryLen) {
#define ICON(id, name, code)          \
    if (strEq(query, queryLen, name)) \
        return code;
#include "icons.inc"
#undef ICON
    return 0;
}

char const *_Mdi__name(uint32_t query) {
#define ICON(id, name, code) \
    if (code == query)       \
        return name;
#include "icons.inc"
#undef ICON
    return 0;
}
