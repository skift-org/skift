#pragma once

#define __packed __attribute__((packed))

#define __aligned(__align) __attribute__((aligned(__align)))

#define __unused(__stuff) (void)(__stuff)

#define __malloc(__type) ((__type *)malloc(sizeof(__type)))
