#include <stdint.h>
#include <string.h>

int strEq(const char *a, size_t aLen, const char *b) {
    return aLen == strlen(b) && memcmp(a, b, aLen) == 0;
}

u32 _Mdi__byName(char const *query, size_t queryLen) {
#define ICON(id, name, code)          \
    if (strEq(query, queryLen, name)) \
        return code;
#include "icons.inc"
#undef ICON
    return 0;
}

char const *_Mdi__name(u32 query) {
#define ICON(id, name, code) \
    if (code == query)       \
        return name;
#include "icons.inc"
#undef ICON
    return 0;
}
