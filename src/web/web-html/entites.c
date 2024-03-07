#include <stddef.h>
#include <string.h>

struct {
    char const *name;
    unsigned int const *runes;
} const ENTITIES[] = {
#define ENTITY(NAME, ...) \
    {#NAME, (unsigned int[]){__VA_ARGS__, 0}},
#include "defs/entities.inc"
#undef ENTITY
};

bool partialMatch(char const *name, size_t len, char const *entity) {
    for (size_t i = 0; i < len; ++i)
        if (name[i] != entity[i])
            return false;
    return true;
}

// Return the runes for the named entity for full matches or if the name starts
// with the entity name
unsigned int const *_Web__Html__lookupEntity(char const *name, size_t len) {
    for (size_t i = 0; i < sizeof(ENTITIES) / sizeof(*ENTITIES); ++i)
        if (partialMatch(name, len, ENTITIES[i].name))
            return ENTITIES[i].runes;
    return NULL;
}
