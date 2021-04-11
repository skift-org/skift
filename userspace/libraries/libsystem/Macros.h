#pragma once

#define PACKED __attribute__((packed))

#define ALIGNED(__align) __attribute__((aligned(__align)))

#define UNUSED(__stuff) (void)(__stuff)

#define CREATE(__type) ((__type *)calloc(1, sizeof(__type)))

#define NO_RETURN __attribute__((noreturn))

#define CLEANUP(__function) __attribute__((__cleanup__(__function)))

#define FLATTEN __attribute__((flatten))

#define ALWAYS_INLINE __attribute__((always_inline))

// Align the nearest _lower_ aligned address
// ex: 8 with align = 8 -> 8
// ex: 9 with align = 8 -> 16
// ex: 7 with align = 8 -> 0
#define ALIGN_DOWN(__addr, __align) ((__addr) & ~((__align)-1))

// Align the nearest _upper_ aligned address
// ex: 8 with align = 8 -> 8
// ex: 9 with align = 8 -> 16
// ex: 7 with align = 8 -> 8
#define ALIGN_UP(__addr, __align) (((__addr) + (__align)-1) & ~((__align)-1))

#define AERAY_LENGTH(__array) (sizeof(__array) / sizeof(__array[0]))

#define NONCOPYABLE(__class_name)              \
    __class_name(const __class_name &) = delete; \
    __class_name &operator=(const __class_name &) = delete;

#define NONMOVABLE(__class_name)                \
    __class_name(const __class_name &&) = delete; \
    __class_name &operator=(const __class_name &&) = delete;
