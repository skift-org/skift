#pragma once

#include <__libc__.h>

#include <stddef.h>
#include <stdint.h>

__BEGIN_HEADER

// Print format macros
#define PRIi8   "hhi"
#define PRIi16  "hi"
#define PRIi32  "li"
#define PRIi64  "lli"

#define PRIx8   "hhx"
#define PRIx16  "hx"
#define PRIx32  "lx"
#define PRIx64  "llx"

#define PRIX8   "hhX"
#define PRIX16  "hX"
#define PRIX32  "lX"
#define PRIX64  "llX"

#define PRIu8     "hhu"
#define PRIu16    "hu"
#define PRIu32    "lu"
#define PRIu64    "llu"

#define PRId8     "hhd"
#define PRId16    "hd"
#define PRId32    "ld"
#define PRId64    "lld"

// Scan format macros
#define SCNi8     "hhi"
#define SCNi16    "hi"
#define SCNi32    "li"
#define SCNi64    "lli"

#define SCNx8     "hhx"
#define SCNx16    "hx"
#define SCNx32    "lx"
#define SCNx64    "llx"

#define SCNX8     "hhX"
#define SCNX16    "hX"
#define SCNX32    "lX"
#define SCNX64    "llX"

#define SCNu8     "hhu"
#define SCNu16    "hu"
#define SCNu32    "lu"
#define SCNu64    "llu"

#define SCNd8     "hhd"
#define SCNd16    "hd"
#define SCNd32    "ld"
#define SCNd64    "lld"

__END_HEADER
