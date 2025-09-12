export module Karm.Image:tga.base;

import Karm.Core;

namespace Karm::Image::Tga {

export enum : usize {
    UNC_CM = 1, // uncompressed color-mapped
    UNC_TC = 2, // uncompressed true-color
    UNC_GS = 3, // uncompressed grayscale

    RLE_CM = 9,  // run-length encoded color-mapped
    RLE_TC = 10, // run-length encoded true-color
    RLE_GS = 11, // run-length encoded grayscale

    ORDER_R2L = (1 << 4), // right-to-left pixel ordering
    ORDER_T2B = (1 << 5), // top-to-bottom pixel ordering

    PACKET_RLE = (1 << 7), // rle/raw field
    PACKET_LEN = 0x7f,     // length mask
};

export struct [[gnu::packed]] Header {
    u8le idLen;
    u8le clrmapType;
    u8le imageType;

    u16le cmIndex;
    u16le cmSize;
    u8le cmBpc;

    u16le originX;
    u16le originY;
    u16le width;
    u16le height;
    u8le bpp;
    u8le desc;
};

} // namespace Karm::Image::Tga
