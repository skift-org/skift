#include <libcompression/CRC.h>

const Array<uint32_t, 256> Compression::CRC::_table = calculate_table<uint32_t, 256>();