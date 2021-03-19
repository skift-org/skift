#pragma once

// See https://tools.ietf.org/html/rfc1951#section-3.2.3
enum BlockType
{
    BT_UNCOMPRESSED = 0,
    BT_FIXED_HUFFMAN = 1,
    BT_DYNAMIC_HUFFMAN = 2,
};