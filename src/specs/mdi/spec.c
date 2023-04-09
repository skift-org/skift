#include <stdint.h>
#include <string.h>

int cstrEq(char const *str1, char const *str2) {
    while (*str1 && *str2) {
        if (*str1++ != *str2++)
            return 0;
    }
    return *str1 == *str2;
}

uint32_t _Mdi__byName(char const *query, size_t) {
#define ICON(id, name, code) \
    if (cstrEq(query, name)) \
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
