#include <abi/Paths.h>
#include <libsystem/io/Stream.h>
#include <libsystem/system/Random.h>

Random random_create()
{
    Random random = {};

    Stream *random_device = stream_open(UNIX_DEVICE_PATH("random"), OPEN_READ);

    stream_read(random_device, &random, sizeof(Random));

    stream_close(random_device);

    return random;
}

uint32_t random_uint32(Random *random)
{
    // *Really* minimal PCG32 code / (c) 2014 M.E. O'Neill / pcg-random.org
    // Licensed under Apache License 2.0 (NO WARRANTY, etc. see website)

    uint64_t oldstate = random->state;
    // Advance internal state
    random->state = oldstate * 6364136223846793005ULL + (random->inc | 1);
    // Calculate output function (XSH RR), uses old state for max ILP
    uint32_t xorshifted = ((oldstate >> 18u) ^ oldstate) >> 27u;
    uint32_t rot = oldstate >> 59u;
    return (xorshifted >> rot) | (xorshifted << ((-rot) & 31));
}

uint32_t random_uint32_max(Random *random, uint32_t max)
{
    return random_uint32(random) % max;
}

#ifndef __KERNEL__

double random_double(Random *random)
{
    return random_uint32(random) / ((double)UINT32_MAX);
}

#endif
