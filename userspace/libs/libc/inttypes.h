#pragma once

#include <__libc__.h>

#include <stddef.h>
#include <stdint.h>

__BEGIN_HEADER

#define PRId8 "hhd"
#define PRId16 "hd"
#define PRId32 "d"
#define PRId64 "ld"

#define PRIdLEAST8 "hhd"
#define PRIdLEAST16 "hd"
#define PRIdLEAST32 "d"
#define PRIdLEAST64 "ld"

#define PRIdFAST8 "hhd"
#define PRIdFAST16 "hd"
#define PRIdFAST32 "d"
#define PRIdFAST64 "ld"

#define PRIi8 "hhi"
#define PRIi16 "hi"
#define PRIi32 "i"
#define PRIi64 "li"

#define PRIiLEAST8 "hhi"
#define PRIiLEAST16 "hi"
#define PRIiLEAST32 "i"
#define PRIiLEAST64 "li"

#define PRIiFAST8 "hhi"
#define PRIiFAST16 "hi"
#define PRIiFAST32 "i"
#define PRIiFAST64 "li"

#define PRIo8 "hho"
#define PRIo16 "ho"
#define PRIo32 "o"
#define PRIo64 "lo"

#define PRIoLEAST8 "hho"
#define PRIoLEAST16 "ho"
#define PRIoLEAST32 "o"
#define PRIoLEAST64 "lo"

#define PRIoFAST8 "hho"
#define PRIoFAST16 "ho"
#define PRIoFAST32 "o"
#define PRIoFAST64 "lo"

#define PRIu8 "hhu"
#define PRIu16 "hu"
#define PRIu32 "u"
#define PRIu64 "lu"

#define PRIuLEAST8 "hhu"
#define PRIuLEAST16 "hu"
#define PRIuLEAST32 "u"
#define PRIuLEAST64 "lu"

#define PRIuFAST8 "hhu"
#define PRIuFAST16 "hu"
#define PRIuFAST32 "u"
#define PRIuFAST64 "lu"

#define PRIx8 "hhx"
#define PRIx16 "hx"
#define PRIx32 "x"
#define PRIx64 "lx"

#define PRIxLEAST8 "hhx"
#define PRIxLEAST16 "hx"
#define PRIxLEAST32 "x"
#define PRIxLEAST64 "lx"

#define PRIxFAST8 "hhx"
#define PRIxFAST16 "hx"
#define PRIxFAST32 "x"
#define PRIxFAST64 "lx"

#define PRIX8 "hhX"
#define PRIX16 "hX"
#define PRIX32 "X"
#define PRIX64 "lX"

#define PRIXLEAST8 "hhX"
#define PRIXLEAST16 "hX"
#define PRIXLEAST32 "X"
#define PRIXLEAST64 "lX"

#define PRIXFAST8 "hhX"
#define PRIXFAST16 "hX"
#define PRIXFAST32 "X"
#define PRIXFAST64 "lX"

#define PRIdMAX "jd"
#define PRIiMAX "ji"
#define PRIoMAX "jo"
#define PRIuMAX "ju"
#define PRIxMAX "jx"
#define PRIXMAX "jX"

#define PRIdPTR "td"
#define PRIiPTR "ti"
#define PRIoPTR "to"
#define PRIuPTR "tu"
#define PRIxPTR "tx"
#define PRIXPTR "tX"

#define SCNd8 PRId8
#define SCNd16 PRId16
#define SCNd32 PRId32
#define SCNd64 PRId64

#define SCNdLEAST8 PRIdLEAST8
#define SCNdLEAST16 PRIdLEAST16
#define SCNdLEAST32 PRIdLEAST32
#define SCNdLEAST64 PRIdLEAST64

#define SCNdFAST8 PRIdFAST8
#define SCNdFAST16 PRIdFAST16
#define SCNdFAST32 PRIdFAST32
#define SCNdFAST64 PRIdFAST64

#define SCNi8 PRIi8
#define SCNi16 PRIi16
#define SCNi32 PRIi32
#define SCNi64 PRIi64

#define SCNiLEAST8 PRIiLEAST8
#define SCNiLEAST16 PRIiLEAST16
#define SCNiLEAST32 PRIiLEAST32
#define SCNiLEAST64 PRIiLEAST64

#define SCNiFAST8 PRIiFAST8
#define SCNiFAST16 PRIiFAST16
#define SCNiFAST32 PRIiFAST32
#define SCNiFAST64 PRIiFAST64

#define SCNo8 PRIo8
#define SCNo16 PRIo16
#define SCNo32 PRIo32
#define SCNo64 PRIo64

#define SCNoLEAST8 PRIoLEAST8
#define SCNoLEAST16 PRIoLEAST16
#define SCNoLEAST32 PRIoLEAST32
#define SCNoLEAST64 PRIoLEAST64

#define SCNoFAST8 PRIoFAST8
#define SCNoFAST16 PRIoFAST16
#define SCNoFAST32 PRIoFAST32
#define SCNoFAST64 PRIoFAST64

#define SCNu8 PRIu8
#define SCNu16 PRIu16
#define SCNu32 PRIu32
#define SCNu64 PRIu64

#define SCNuLEAST8 PRIuLEAST8
#define SCNuLEAST16 PRIuLEAST16
#define SCNuLEAST32 PRIuLEAST32
#define SCNuLEAST64 PRIuLEAST64

#define SCNuFAST8 PRIuFAST8
#define SCNuFAST16 PRIuFAST16
#define SCNuFAST32 PRIuFAST32
#define SCNuFAST64 PRIuFAST64

#define SCNx8 PRIx8
#define SCNx16 PRIx16
#define SCNx32 PRIx32
#define SCNx64 PRIx64

#define SCNxLEAST8 PRIxLEAST8
#define SCNxLEAST16 PRIxLEAST16
#define SCNxLEAST32 PRIxLEAST32
#define SCNxLEAST64 PRIxLEAST64

#define SCNxFAST8 PRIxFAST8
#define SCNxFAST16 PRIxFAST16
#define SCNxFAST32 PRIxFAST32
#define SCNxFAST64 PRIxFAST64

#define SCNdMAX PRIdMAX
#define SCNiMAX PRIiMAX
#define SCNoMAX PRIoMAX
#define SCNuMAX PRIuMAX
#define SCNxMAX PRIxMAX

#define SCNdPTR PRIdPTR
#define SCNiPTR PRIiPTR
#define SCNoPTR PRIoPTR
#define SCNuPTR PRIuPTR
#define SCNxPTR PRIxPTR

__END_HEADER